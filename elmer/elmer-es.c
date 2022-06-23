/*
  Licence: GPLv3
  Copyright Ⓒ 2022 Valerie Pond
  Elmer

  Force a user to speak like Elmer Fudd

  Ported from angrywolf's module:
  https://web.archive.org/web/20161126194547/http://www.angrywolf.org/modules.php
  
  Special thanks to Jobe for helping me with casting =]
*/
/*** <<<MODULE MANAGER START>>>
module
{
        documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/elmer/README.md";
		troubleshooting "In case of problems, documentation or e-mail me at v.a.pond@outlook.com";
        min-unrealircd-version "6.*";
        max-unrealircd-version "6.*";
        post-install-text {
                "The module is installed. Now all you need to do is add a loadmodule line:";
                "loadmodule \"third/elmer\";";
                "And /REHASH the IRCd.";
                "The module does not need any other configuration.";
        }
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"

ModuleHeader MOD_HEADER = {
	"third/elmer-es",
	"2.1",
	"Haz que la gente hable raro (Elmer).",
	"Valware",
	"unrealircd-6",
};


#define IsElmer(x)			(moddata_client(x, elmer_md).i)
#define SetElmer(x)		do { moddata_client(x, elmer_md).i = 1; } while(0)
#define ClearElmer(x)		do { moddata_client(x, elmer_md).i = 0; } while(0)

#define ADDELM "ELMER"
#define DELELM "DELMER"

CMD_FUNC(ADDELMER);
CMD_FUNC(DELELMER);

void elmer_free(ModData *m);
const char *elmer_serialize(ModData *m);
void elmer_unserialize(const char *str, ModData *m);

static char *convert_to_elmer(char *line);

int elmer_chanmsg(Client *client, Channel *channel, Membership *lp, const char **msg, const char **errmsg, SendType sendtype);
int elmer_usermsg(Client *client, Client *target, const char **msg, const char **errmsg, SendType sendtype);

static void dumpit(Client *client, char **p);

static char *help_elmer[] = {
	"***** Elmer *****",
	"-",
	"Obliga a otro usuario (o a ti mismo) a hablar como",
	"Elmer Fudd. (Reemplaza 'r's y 'l's con 'w's)",
	"-",
	"Syntax:",
	"    /ELMER [-lista|-ayuda|<nick>]",
	"    /DELMER <nick>",
	"-",
	"Ejemplos:",
	"-",
	"Lista de usuarios que son Elmer (Oper):",
	"    /ELMER -list",
	"-",
	"Ver esta salida:",
	"    /ELMER -help",
	"-",
	"Agregue un usuario cuyo nick sea Lamer32 como Elmer:",
	"    /ELMER Lamer32",
	"-",
	"Eliminar un usuario cuyo nick es Lamer32 como Elmer:",
	"    /DELMER Lamer32",
	"-",
	NULL
};
ModDataInfo *elmer_md;



MOD_INIT() {
	ModDataInfo mreq;

	MARK_AS_GLOBAL_MODULE(modinfo);
	
	memset(&mreq, 0, sizeof(mreq));
	mreq.name = "elmer";
	mreq.free = elmer_free;
	mreq.serialize = elmer_serialize;
	mreq.unserialize = elmer_unserialize;
	mreq.sync = 1;
	mreq.type = MODDATATYPE_CLIENT;
	elmer_md = ModDataAdd(modinfo->handle, mreq);
	if (!elmer_md)
		abort();
	
	CommandAdd(modinfo->handle, ADDELM, ADDELMER, 1, CMD_USER);
	CommandAdd(modinfo->handle, DELELM, DELELMER, 1, CMD_USER);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_CHANNEL, 0, elmer_chanmsg);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_USER, 0, elmer_usermsg);
	
	/* adding a fkn /HELP command output XD */
	//add_elmer_help();
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
const char *elmer_serialize(ModData *m)
{
	static char buf[32];
	if (m->i == 0)
		return NULL; /* not set */
	snprintf(buf, sizeof(buf), "%d", m->i);
	return buf;
}
void elmer_free(ModData *m)
{
    m->i = 0;
}
void elmer_unserialize(const char *str, ModData *m)
{
    m->i = atoi(str);
}

static void dumpit(Client *client, char **p) {
	if(IsServer(client))
		return;
	for(; *p != NULL; p++)
		sendto_one(client, NULL, ":%s %03d %s :%s", me.name, RPL_TEXT, client->name, *p);
}

CMD_FUNC(ADDELMER)
{
	Client *target;
	int self = 0;
	int operclient;
	if (parc < 2 || !strcasecmp(parv[1],"-ayuda"))
	{
		dumpit(client,help_elmer);
		return;
	}
	else if (!strcasecmp(parv[1],"-lista") && IsOper(client))
	{
		int found = 0;
		int listnum = 1;
		sendnotice(client,"Listado de todos los marcados como 'Elmer':");
		list_for_each_entry(target, &client_list, client_node)
		{
			if (!IsServer(target) && IsElmer(target))
			{
				found = 1;
				sendnotice(client,"%d) %s", listnum, target->name);
				listnum++;
			}
		}
		if (!found)
				sendnotice(client,"La ELMER lista esta vacía.");
		return;
	}
	else if (!(target = find_user(parv[1], NULL))) {
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	operclient = IsOper(client) ? 1 : 0; // ¿are we an oper?

	if (IsOper(target) && client != target)
	{
		// huhuhuhu... lets let it get sent to ulines tho
		if (!IsULine(target) && operclient) 
		{
			sendnumeric(client, ERR_NOPRIVILEGES);
			return;
		}	
	}
	if (!operclient && client == target) // client is not oper but wants to elmer themselves
		self++;
	
	else if (!operclient)
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	
	if (IsElmer(target))
	{
		sendto_one(target, NULL, "FAIL %s %s %s :%s", ADDELM, "USER_ALREADY_ELMER",target->name,"Persona ya está marcada como 'Elmer'.");
		return;
	}
	/* if they set it on themselves from not being an oper, they won't see the log snotice, so we should inform them manually
	 * reason we do this is because they will see either one or the other. we don't like duplicate confirmations, but we don't leave them out.
	 */
	if (self) // if they set it on themselves from not being an oper, they won't see the log snotice, so we should inform them manually
		sendnotice(client,"Ahora estás marcado como 'Elmer'");

	SetElmer(target);
	
	unreal_log(ULOG_INFO, "elmer", "ADD_ELMER", client, "$client ha marcado a $target como 'Elmer'.",log_data_string("client", client->name),log_data_string("target", target->name));
	return;
}

CMD_FUNC(DELELMER)
{
	Client *target;
	int self = 0;
	int operclient;
	if (parc < 2 || !strcasecmp(parv[1],"-ayuda"))
	{
		dumpit(client,help_elmer);
		return;
	}
	else if (!(target = find_user(parv[1], NULL))) {
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	operclient = IsOper(client) ? 1 : 0; // ¿are we an oper?

	if (IsOper(target) && client != target)
	{
		// huhuhuhu... lets let it get sent to ulines tho
		if (!IsULine(target) && operclient) 
		{
			sendnumeric(client, ERR_NOPRIVILEGES);
			return;
		}	
	}
	if (!operclient && client == target) // client is not oper but wants to delmer themselves
		self++;
	
	else if (!operclient)
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	if (!IsElmer(target))
	{
		sendto_one(target, NULL, "FAIL %s %s %s :%s", DELELM, "USER_NOT_ELMER",target->name,"La persona no está marcada como 'Elmer' de todos modos.");
		return;
	}
	/* if they unset it from themselves from not being an oper, they won't see the log snotice, so we should inform them manually
	 * reason we do this is because they will see either one or the other. we don't like duplicate confirmations, but we don't leave them out.
	 */
	if (self) 
		sendnotice(client,"Ya no estás marcado como 'Elmer'.");
		
	unreal_log(ULOG_INFO, "elmer", "DEL_ELMER", client, "$client ha eliminado a la $target de estar marcada como 'Elmer'.",log_data_string("client", client->name),log_data_string("target", target->name));
	ClearElmer(target);
	return;
}

int elmer_chanmsg(Client *client, Channel *channel, Membership *lp, const char **msg, const char **errmsg, SendType sendtype)
{
	static char retbuf[512];
	if (IsElmer(client))
	{
		strlcpy(retbuf, *msg, sizeof(retbuf));
		*msg = convert_to_elmer(retbuf);
	}
	return 0;
}

int elmer_usermsg(Client *client, Client *target, const char **msg, const char **errmsg, SendType sendtype)
{
	static char retbuf[512];
	if (IsElmer(client) && !IsULine(target))
	{
		strlcpy(retbuf, *msg, sizeof(retbuf));
		*msg = convert_to_elmer(retbuf);
	}
	return 0;
}

static char *convert_to_elmer(char *line)
{
	char *p;
	for (p = line; *p; p++)
		switch(*p)
		{
			case 's':
				*p = 'f';
				break;
			case 'S':
				*p = 'F';
				break;
			case 't':
				*p = 'd';
				break;
			case 'T':
				*p = 'D';
				break;
		}

	return line;
}
