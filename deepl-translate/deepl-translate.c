/*
  Licence: GPLv3 or later
  Copyright Ⓒ 2024 Valerie Pond
  
*/
/*** <<<MODULE MANAGER START>>>
module
{
		documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/deepl-translate/README.md";
		troubleshooting "In case of problems, documentation or e-mail me at v.a.pond@outlook.com";
		min-unrealircd-version "6.1.4";
		max-unrealircd-version "6.*";
		post-install-text {
				"The module is installed. Now all you need to do is add a loadmodule line:";
				"loadmodule \"third/deepl-translate\";";
				"And /REHASH the IRCd.";
		}
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"

#define MTAG_ISO_LANG "+valware.uk/iso-lang"
#define DEEPL_TR_CONF "deepl-translate"

#define SetLanguage(x, y)	do { moddata_client_set(x, "language", y); } while (0)
#define GetLanguage(x) moddata_client_get(x, "language")

long extumode_deepl_translate = 0L;
#define UMODE_DEEPL_TRANSLATE 'A'
#define IsAutoTranslateUser(x) (IsUser(x) && has_user_mode(x, 'A'))

Cmode_t EXTCMODE_DEEPL_TRANSLATE;
#define CMODE_DEEPL_TRANSLATE 'A'
#define IsAutoTranslateChannel(channel)	(channel->mode.mode & EXTCMODE_DEEPL_TRANSLATE)

/* Config struct*/
struct cfgstruct {
	char *url;
	char *key;

	unsigned short int got_url;
	unsigned short int got_key;
};
static struct cfgstruct cfg;

typedef struct {
	char *target;		// Channel or user to send the translated message
	char *from_user;	// Original user who sent the message
	char *original_text;	// Original message text
	char *language;		// Target language for translation

	/* response */
	char *from_language;
} TranslationRequest;

void free_translation_request(TranslationRequest *tr)
{
	safe_free(tr->target);
	safe_free(tr->from_user);
	safe_free(tr->original_text);
	safe_free(tr->language);
	safe_free(tr->from_language);
	safe_free(tr);
}

typedef struct ISO_Language {
	struct ISO_Language *prev, *next;
	char code[8];
} ISO_Language;

// Define a structure to hold ISO language information
typedef struct {
	char code[8];
	char name[70];
} ISO_Language_List;

int deepl_translate_configtest(ConfigFile *cf, ConfigEntry *ce, int type, int *errs);
int deepl_translate_configrun(ConfigFile *cf, ConfigEntry *ce, int type);
int deepl_translate_channel_message(Client *client, Channel *channel, int sendflags, const char *member_modes,
									const char *target, MessageTag *mtags, const char *text, SendType sendtype);
void channel_translation_download_complete(OutgoingWebRequest *request, OutgoingWebResponse *response);

const ISO_Language_List iso_language_list[] = {
	{ "AR", "Arabic" },
	{ "BG", "Bulgarian" },
	{ "CS", "Czech" },
	{ "DA", "Danish" },
	{ "DE", "German" },
	{ "EL", "Greek" },
	{ "EN-GB", "English (British)" },
	{ "EN-US", "English (American)" },
	{ "ES", "Spanish" },
	{ "ET", "Estonian" },
	{ "FI", "Finnish" },
	{ "FR", "French" },
	{ "HU", "Hungarian" },
	{ "ID", "Indonesian" },
	{ "IT", "Italian" },
	{ "JA", "Japanese" },
	{ "KO", "Korean" },
	{ "LT", "Lithuanian" },
	{ "LV", "Latvian" },
	{ "NB", "Norwegian (Bokmål)" },
	{ "NL", "Dutch" },
	{ "PL", "Polish" },
	{ "PT-BR", "Portuguese (Brazilian)" },
	{ "PT-PT", "Portuguese (all Portuguese varieties excluding Brazilian Portuguese)" },
	{ "RO", "Romanian" },
	{ "RU", "Russian" },
	{ "SK", "Slovak" },
	{ "SL", "Slovenian" },
	{ "SV", "Swedish" },
	{ "TR", "Turkish" },
	{ "UK", "Ukrainian" },
	{ "ZH", "Chinese (simplified)" }
};

// Function to check if a given string is a valid ISO language code
bool isValidISOLanguageCode(const char *code) {
	int i;
	for (i = 0; i < sizeof(iso_language_list) / sizeof(iso_language_list[0]); i++) {
		if (strcmp(code, iso_language_list[i].code) == 0) {
			return true;
		}
	}
	return false;
}

ISO_Language* create_language_node(const char *code) {
	ISO_Language *new_node = safe_alloc(sizeof(ISO_Language));
	if (new_node == NULL) {
		return NULL; // Memory allocation failed
	}
	strncpy(new_node->code, code, sizeof(new_node->code) - 1);
	new_node->code[sizeof(new_node->code) - 1] = '\0';
	new_node->prev = NULL;
	new_node->next = NULL;
	return new_node;
}
void free_language_list(ISO_Language *head) {
	ISO_Language *current = head, *next;

	while (current != NULL)
	{
		next = current->next;
		free(current->code);
		free(current);
		current = next;
	}
}
// Function to check if a language code is already in the list
int list_has_language(ISO_Language *list, const char *code) {
	ISO_Language *current;
	for (current = list; current != NULL; current = current->next) {
		if (strcmp(current->code, code) == 0) {
			return 1;
		}
	}
	return 0;
}

ISO_Language *get_languages_to_translate_from_channel(Channel *channel)
{
	ISO_Language *lang_head = NULL;
	Member *member;
	Client *client;
	const char *lang = NULL;

	for (member = channel->members; member; member = member->next)
	{
		client = member->client;
		lang = GetLanguage(client);
		if (lang && isValidISOLanguageCode(lang)
			&& !list_has_language(lang_head, lang)
			&& IsAutoTranslateUser(client))
		{
			ISO_Language *new_node = create_language_node(lang);
			if (!new_node)
			{
				unreal_log(ULOG_ERROR, "deepl-translate", "FATAL_MEMORY_ALLOCATION_ERROR", NULL, "Could not allocate memory in channel translation routine");
				continue;
			}
			AddListItem(new_node, lang_head);
		}
	}
	return lang_head;
}


int iso_lang_mtag_is_ok(Client *client, const char *name, const char *value)
{
	if (isValidISOLanguageCode(value))
		return 1;

	sendto_one(client, NULL, ":%s NOTE * INVALID_MTAG_VALUE %s :Value for tag \"%s\" must be a valid ISO language code, for example en-GB", me.name, value, name);
	return 0;
}

void mtag_add_iso_lang(Client *client, MessageTag *recv_mtags, MessageTag **mtag_list, const char *signature)
{
	MessageTag *m;

	if (IsUser(client))
	{
		m = find_mtag(recv_mtags, MTAG_ISO_LANG);
		if (m)
		{
			m = duplicate_mtag(m);
			AddListItem(m, *mtag_list);
		}
	}
}

void language_free(ModData *m)
{
	safe_free(m->str);
}

const char *language_serialize(ModData *m)
{
	if (!m->str)
		return NULL;
	return m->str;
}

void language_unserialize(const char *str, ModData *m)
{
	safe_strdup(m->str, str);
}

ModuleHeader MOD_HEADER = {
	"third/deepl-translate",
	"1.0",
	"Text translation using DeepL Translate",
	"Valware",
	"unrealircd-6",
};

CMD_FUNC(cmd_setlang)
{
	if (BadPtr(parv[1]) || !isValidISOLanguageCode(parv[1]))
	{
		sendto_one(client, NULL, ":%s FAIL SETLANG INVALID_ISO_LANG %s :Please specify a valid ISO language code, for example /SETLANG en-GB", me.name, BadPtr(parv[1]) ? "" : parv[1]);
		return;
	}
	moddata_client_set(client, "language", parv[1]);
	sendnotice(client, "You have updated your chosen incoming translation language to be \"%s\"", moddata_client_get(client, "language") ? moddata_client_get(client, "language") : "<none>");
}


MOD_INIT()
{
	MessageTagHandlerInfo mtag;
	ModDataInfo mreq;
	CmodeInfo req;
	MARK_AS_GLOBAL_MODULE(modinfo);

	memset(&mreq, 0, sizeof(mreq));
	mreq.name = "language";
	mreq.free = language_free;
	mreq.serialize = language_serialize;
	mreq.unserialize = language_unserialize;
	mreq.type = MODDATATYPE_CLIENT;
	if (!ModDataAdd(modinfo->handle, mreq))
	{
		config_error("Could not add ModData for \"language\". Please contact developer (Valware).");
		return MOD_FAILED;
	}

	memset(&req, 0, sizeof(req));
	req.paracount = 0;
	req.letter = CMODE_DEEPL_TRANSLATE;
	req.is_ok = extcmode_default_requirechop; // require that the user is at least channel halfop
	CmodeAdd(modinfo->handle, req, &EXTCMODE_DEEPL_TRANSLATE);

	memset(&mtag, 0, sizeof(mtag));
	mtag.is_ok = iso_lang_mtag_is_ok;
	mtag.flags = MTAG_HANDLER_FLAGS_NO_CAP_NEEDED;
	mtag.name = MTAG_ISO_LANG;
	if (!MessageTagHandlerAdd(modinfo->handle, &mtag))
	{
		config_error("Could not add MessageTag \"%s\". Please contact developer (Valware).", MTAG_ISO_LANG);
		return MOD_FAILED;
	}

	UmodeAdd(modinfo->handle, UMODE_DEEPL_TRANSLATE, UMODE_GLOBAL, 0, NULL, &extumode_deepl_translate);
	HookAdd(modinfo->handle, HOOKTYPE_CHANMSG, 0, deepl_translate_channel_message);
	RegisterApiCallbackWebResponse(modinfo->handle, "channel_translation_download_complete", channel_translation_download_complete);
	HookAdd(modinfo->handle, HOOKTYPE_CONFIGRUN, 0, deepl_translate_configrun);
	HookAddVoid(modinfo->handle, HOOKTYPE_NEW_MESSAGE, 0, mtag_add_iso_lang);

	CommandAdd(modinfo->handle, "SETLANG", cmd_setlang, 1, CMD_USER | CMD_UNREGISTERED);
	return MOD_SUCCESS;
}

MOD_TEST()
{
	HookAdd(modinfo->handle, HOOKTYPE_CONFIGTEST, 0, deepl_translate_configtest);
	return MOD_SUCCESS;
}

/** Called upon module load */
MOD_LOAD()
{
	return MOD_SUCCESS;
}
/** Called upon unload */
MOD_UNLOAD()
{
	return MOD_SUCCESS;
}

int deepl_translate_configtest(ConfigFile *cf, ConfigEntry *ce, int type, int *errs)
{
	int errors = 0; // Error count
	ConfigEntry *cep; // To store the current variable/value pair etc, nested

	// Since we'll add a top-level block to unrealircd.conf, need to filter on CONFIG_MAIN lmao
	if (type != CONFIG_MAIN)
		return 0; // Returning 0 means idgaf bout dis

	// Check for valid config entries first
	if (!ce || !ce->name)
		return 0;

	// If it isn't our block, idc
	if (strcmp(ce->name, DEEPL_TR_CONF))
		return 0;

	// Loop dat shyte
	for (cep = ce->items; cep; cep = cep->next)
	{
		// Do we even have a valid name l0l?
		// This should already be checked by Unreal's core functions but there's no harm in having it here too =]
		if (!cep->name)
		{
			config_error("[deepl-translate] %s:%i: blank %s item", cep->file->filename, cep->line_number, DEEPL_TR_CONF); // Rep0t error
			errors++; // Increment err0r count
			continue; // Next iteration imo tbh
		}

		if (!strcmp(cep->name, "api-url"))
		{
			if (cfg.got_url)
			{
				config_error("[deepl-translate] %s:%i: duplicate %s::%s directive. Only one URL is supported at this time.", cep->file->filename, cep->line_number, DEEPL_TR_CONF, cep->name);
				errors++;
				continue;
			}

			cfg.got_url = 1;
			if (!strlen(cep->value))
			{
				config_error("[deepl-translate] %s:%i: %s::%s must be non-empty", cep->file->filename, cep->line_number, DEEPL_TR_CONF, cep->name);
				errors++;
				continue;
			}

			continue;
		}

		if (!strcmp(cep->name, "api-key"))
		{
			if (cfg.got_key)
			{
				config_error("[deepl-translate] %s:%i: duplicate %s::%s directive", cep->file->filename, cep->line_number, DEEPL_TR_CONF, cep->name);
				errors++;
				continue;
			}

			cfg.got_key = 1;
			if (!strlen(cep->value))
			{
				config_error("[deepl-translate] %s:%i: %s::%s must be non-empty", cep->file->filename, cep->line_number, DEEPL_TR_CONF, cep->name);
				errors++;
				continue;
			}
			continue;
		}
		config_warn("[deepl-translate] %s:%i: unknown item %s::%s", cep->file->filename, cep->line_number, DEEPL_TR_CONF, cep->name);
	}

	if (!cfg.got_key || !cfg.got_url)
	{
		config_error("[deepl-translate] You have not set your \"deepl-translate {}\" block correctly. Please read the documentation.");
		errors++;
	}
	*errs = errors;
	return errors ? -1 : 1;
}

int deepl_translate_configrun(ConfigFile *cf, ConfigEntry *ce, int type)
{
	ConfigEntry *cep;
	
	if (type != CONFIG_MAIN)
		return 0;

	if (!ce || !ce->name)
		return 0;

	if (strcmp(ce->name, DEEPL_TR_CONF))
		return 0;

	for (cep = ce->items; cep; cep = cep->next)
	{
		if (!cep->name)
			continue;

		if (!strcmp(cep->name, "api-url"))
		{
			safe_strdup(cfg.url, cep->value);
			continue;
		}

		if (!strcmp(cep->name, "api-key"))
		{
			safe_strdup(cfg.key, cep->value);
			continue;
		}
	}
	return 1; // We good
}


/* 	run the translation request through DeepL 
	See https://developers.deepl.com/docs/api-reference/translate
*/
int deepl_translate_channel_message(Client *client, Channel *channel, int sendflags, const char *member_modes,
									const char *target, MessageTag *mtags, const char *text, SendType sendtype)
{
	if (sendtype == SEND_TYPE_TAGMSG || !strlen(text))
		return 0;
		
	ISO_Language *language_list, *l;
	if (!IsAutoTranslateChannel(channel))
		return 0;

	ISO_Language *lang_head = NULL;
	Member *member;
	Client *m;
	char checked[1000];

	for (member = channel->members; member; member = member->next)
	{
		char lang[10] = "\0";
		m = member->client;
		if (!strcmp(m->id, client->id))
			continue;
		snprintf(lang, sizeof(lang), "%s", moddata_client_get(m, "language") ? moddata_client_get(m, "language") : "<none>");

		
		if (strcmp(lang, "<none>")
			&& isValidISOLanguageCode(lang)
			&& !list_has_language(lang_head, lang)
			&& IsAutoTranslateUser(m))
		{

			/* Avoid duplicated stuff */
			char temp[1000];
    		snprintf(temp, sizeof(temp), "%s,", lang);
			if (strstr(checked, temp))
				continue;

			if ((strlen(lang) + strlen(checked) + 2) > 1000) // too big
				return 0;

			/* make a note that we already checked this language */
			strcat(checked, lang);
			strcat(checked, ",");


			/** JSON needs to be like:
				{
					"text": [
						"我想要乌龙茶",
						"进而...",
						"..."
					],
					"target_lang": "en-US"
				}
			*/
			json_t *object = json_object();
			json_t *txt = json_array();
			json_array_append_new(txt, json_string_unreal(text));
			json_object_set_new(object, "text", txt);
			json_object_set_new(object, "target_lang", json_string_unreal(lang));
			char *body = json_dumps(object, 0);

			TranslationRequest *tr = safe_alloc(sizeof(TranslationRequest));
			safe_strdup(tr->target, channel->name);
			safe_strdup(tr->from_user, client->id);
			tr->original_text = strdup(text);
			safe_strdup(tr->language, lang);

			OutgoingWebRequest *w = safe_alloc(sizeof(OutgoingWebRequest));

			NameValuePrioList *headers = NULL;
			char headervalue[128] = "DeepL-Auth-Key ";
			strcat(headervalue, cfg.key);

			add_nvplist(&headers, 0, "Content-Type", "application/json");
			add_nvplist(&headers, 0, "Authorization", headervalue);

			safe_strdup(w->url, cfg.url);
			w->http_method = HTTP_METHOD_POST;
			w->body = body;
			w->headers = headers;
			w->max_redirects = 1;
			w->callback_data = tr;
			safe_strdup(w->apicallback, "channel_translation_download_complete"); // didn't make the callback function yet

			url_start_async(w);

			json_decref(txt);
			json_decref(object);
		}
	}

	return 0;
}
// {"translations":[{"detected_source_language":"EN","text":"así que ahora sólo traducirá frases completas"}]}
void channel_translation_download_complete(OutgoingWebRequest *request, OutgoingWebResponse *response)
{
	json_t *result;
	json_error_t jerr;

	if (response->errorbuf || !response->memory)
	{
		unreal_log(ULOG_WARNING, "deepl-translate", "ERROR_TALKING_TO_API", NULL, "%s", "Error while trying to check $url: $err",
				   log_data_string("url", request->url), log_data_string("err", response->errorbuf));
		return;
	}
	TranslationRequest *tr = (TranslationRequest *)request->callback_data;
	Client *client = find_user(tr->from_user, NULL);
	if (!client) // sending client no longer online, cannot fulfil request
		return;
	result = json_loads(response->memory, JSON_REJECT_DUPLICATES, &jerr);
	if (!result)
	{
		unreal_log(ULOG_INFO, "deepl-translate", "API_BAD_RESPONSE", NULL,
				   "Error while trying to check $url: JSON parse error",
				   log_data_string("url", request->url));
		return;
	}

	json_t *data = json_object_get(result, "translations");
	json_t *translations = json_array_get(data, 0);
	const char *detected_language = json_string_value(json_object_get(translations, "detected_source_language"));
	const char *text = json_string_value(json_object_get(translations, "text"));

	Channel *chan = find_channel(tr->target);
	if (!chan) // target no longer exists
	{
		json_decref(result);
		return;
	}

	Member *member;
	for (member = chan->members; member; member = member->next)
	{
		Client *m = member->client;
		if (!strcmp(client->id, m->id))
			continue;
			
		if (moddata_client_get(m, "language"))
		{
			int f = !strncmp(tr->language, moddata_client_get(m, "language"), 2);
			if (f)
			{
				sendto_one(m, NULL, ":%s!%s@%s PRIVMSG %s :%s", client->name, client->user->username, client->user->cloakedhost, chan->name, text);
			}
		}
	}

	json_decref(result);
	free_translation_request(tr);
}
