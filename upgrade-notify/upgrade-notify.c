
/*** <<<MODULE MANAGER START>>>
module
{
		documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/upgrade-notify/README.md";
		troubleshooting "In case of problems, documentation or e-mail me at v.a.pond@outlook.com";
		min-unrealircd-version "6.*";
		max-unrealircd-version "6.*";
		post-install-text {
				"The module is installed. Now all you need to do is add a loadmodule line:";
				"loadmodule \"third/upgrade-notify\";";
				"And /REHASH the IRCd.";
				"The module does not need any other configuration.";
		}
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"

CMD_FUNC(cmd_checknew);

/* Place includes here */
#define MSG_CHECKNEW "CHECKNEW"
#define TWICE_PER_DAY 43200000
EVENT(check_for_updates);

ModuleHeader MOD_HEADER
= {
	"third/upgrade-notify",	/* Name of module */
	"1.0", /* Version */
	"Sends out a message to opers when there is an upgrade available for UnrealIRCd", /* Short description of module */
	"Valware",
	"unrealircd-6",
};

MOD_INIT()
{
	EventAdd(modinfo->handle, "check_for_updates", check_for_updates, NULL, TWICE_PER_DAY, 0); // once per day
	return MOD_SUCCESS;
}

/* Is first run when server is 100% ready */
MOD_LOAD()
{
	return MOD_SUCCESS;
}

/* Called when module is unloaded */
MOD_UNLOAD()
{
	return MOD_SUCCESS;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t real_size = size * nmemb;
	char **response_ptr = (char **)userp;

	// Reallocate memory to accommodate new data
	*response_ptr = realloc(*response_ptr, real_size + 1);

	if (*response_ptr == NULL) {
		printf("Memory allocation failed\n");
		return 0;
	}

	// Copy data into the response buffer
	memcpy(*response_ptr, contents, real_size);
	(*response_ptr)[real_size] = 0; // Null-terminate the string

	return real_size;
}

EVENT(check_for_updates)
{
	CURL *curl;
	CURLcode res;
	char *url = "https://www.unrealircd.org/downloads/list.json";
	char *response = NULL;
	char *version_string = NULL;
	int upgrade_available = 0;
	version_string = (char *)malloc(strlen(version) + 1);
	strcpy(version_string, version);
	char *tok = strtok(version_string, "-");
	char *current_version = "\0";

	while (tok != NULL)
	{
		// Check if the tok contains only digits and dots
		int is_numeric = 1;
		for (int i = 0; tok[i] != '\0'; i++) {
			if (!isdigit(tok[i]) && tok[i] != '.') {
				is_numeric = 0;
				break;
			}
		}

		// If the tok contains only digits and dots, it's the numbered string
		if (is_numeric) {
			current_version = tok;
			break; // Exit the loop once found
		}

		// Get the next tok
		tok = strtok(NULL, "-");
	}


	// Initialize libcurl
	curl = curl_easy_init();
	if (!curl)
		return;

	// Set the URL
	curl_easy_setopt(curl, CURLOPT_URL, url);

	// Set the callback function to handle the fetched data
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	// Perform the request
	res = curl_easy_perform(curl);

	// Check for errors
	if (res != CURLE_OK)
		return;

	// Parse JSON using Jansson
	json_error_t error;
	json_t *root = json_loads(response, 0, &error);

	// Check for JSON parsing errors
	if (!root)
	{
		return;
	}
	if (!json_is_object(root))
	{
		return;
	}
	const char *key;
	json_t *value;
	json_object_foreach(root, key, value)
	{
		if (!json_is_object(value))
			return;

		const char *key2;
		json_t *value2;
		json_object_foreach(value, key2, value2)
		{
			if (!strcmp(key2,"Stable"))
			{
				const char *key3;
				json_t *value3;
				json_object_foreach(value2, key3, value3)
				{
					if (!strcmp(key3, "version"))
					{
						const char *stable_version = json_string_value(value3);
						int result = strcmp(current_version, stable_version);
						if (result < 0)
						{
							unreal_log(ULOG_INFO, "upgrade", "UPGRADE_AVAILABLE", NULL, "There is an upgrade available for UnrealIRCd! Your version: UnrealIRCd %s - New version: UnrealIRCd %s", current_version, stable_version);
							unreal_log(ULOG_INFO, "upgrade", "UPGRADE_AVAILABLE", NULL, "Visit https://www.unrealircd.org/docs/Upgrading for information on upgrading.");
						}
						break;
					}
				}
			}
		}
	}
	free(version_string);
	json_decref(root);
	curl_easy_cleanup(curl);
	free(response);
}
