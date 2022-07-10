/*
  Licence: GPLv3
  Copyright Ⓒ 2022 Valerie Pond
  
  Permissions: By using this module, you agree that it (this module) is Free, and you are allowed to make copies
  and redistrubite this at your own free will, so long as in doing so, the original author and license remain in-tact. 


  Force a user to look like they are trying to type while drunk

*/
/*** <<<MODULE MANAGER START>>>
module
{
		documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/drunktalk/README.md";
		troubleshooting "In case of problems, documentation or e-mail me at v.a.pond@outlook.com";
		min-unrealircd-version "6.*";
		max-unrealircd-version "6.*";
		post-install-text {
				"The module is installed. Now all you need to do is add a loadmodule line:";
				"loadmodule \"third/drunktalk\";";
				"And /REHASH the IRCd.";
				"The module does not need any other configuration.";
		}
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"

ModuleHeader MOD_HEADER = {
	"third/drunktalk",
	"1.0",
	"Make people talk as if they are drunk",
	"Valware",
	"unrealircd-6",
};


#define IsDrunk(x)			(moddata_client(x, drunk_md).i)
#define SetDrunk(x)		do { moddata_client(x, drunk_md).i = 1; } while(0)
#define ClearDrunk(x)		do { moddata_client(x, drunk_md).i = 0; } while(0)

#define CMD_DRUNK "DRUNK"
#define CMD_SOBER "SOBER"

CMD_FUNC(ADDDRUNK);
CMD_FUNC(DELDRUNK);

void drunk_free(ModData *m);
const char *drunk_serialize(ModData *m);
void drunk_unserialize(const char *str, ModData *m);

static char *convert_to_drunk(char *line);
int drunk_chanmsg(Client *client, Channel *channel, Membership *lp, const char **msg, const char **errmsg, SendType sendtype);
int drunk_usermsg(Client *client, Client *target, const char **msg, const char **errmsg, SendType sendtype);
int random_number(int low, int high);
static void dumpit(Client *client, char **p);

static char *help_drunk[] = {
	"***** Drunk *****",
	"-",
	"Forces another user to talk as if they are drunk.",
	"-",
	"Syntax:",
	"	/DRUNK [-list|-help|<nick>]",
	"	/SOBER <nick>",
	"-",
	"Examples:",
	"-",
	"List users who are Drunk (Oper):",
	"	/DRUNK -list",
	"-",
	"View this output:",
	"	/DRUNK -help",
	"-",
	"Add a user whose nick is Lamer32 as Drunk:",
	"	/DRUNK Lamer32",
	"-",
	"Remove a user whose nick is Lamer32 as Drunk:",
	"	/SOBER Lamer32",
	"-",
	NULL
};
ModDataInfo *drunk_md;



MOD_INIT() {
	ModDataInfo mreq;

	MARK_AS_GLOBAL_MODULE(modinfo);
	
	memset(&mreq, 0, sizeof(mreq));
	mreq.name = "drunk";
	mreq.free = drunk_free;
	mreq.serialize = drunk_serialize;
	mreq.unserialize = drunk_unserialize;
	mreq.sync = MODDATA_SYNC_EARLY;
	mreq.remote_write = 1;
	mreq.type = MODDATATYPE_CLIENT;
	drunk_md = ModDataAdd(modinfo->handle, mreq);
	if (!drunk_md)
		abort();
	
	CommandAdd(modinfo->handle, CMD_DRUNK, ADDDRUNK, 1, CMD_USER);
	CommandAdd(modinfo->handle, CMD_SOBER, DELDRUNK, 1, CMD_USER);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_CHANNEL, 0, drunk_chanmsg);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_USER, 0, drunk_usermsg);
	
	/* adding a fkn /HELP command output XD */
	//add_drunk_help();
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
const char *drunk_serialize(ModData *m)
{
	static char buf[32];
	if (m->i == 0)
		return NULL; /* not set */
	snprintf(buf, sizeof(buf), "%d", m->i);
	return buf;
}
void drunk_free(ModData *m)
{
	m->i = 0;
}
void drunk_unserialize(const char *str, ModData *m)
{
	m->i = atoi(str);
}

static void dumpit(Client *client, char **p) {
	if(IsServer(client))
		return;
	for(; *p != NULL; p++)
		sendto_one(client, NULL, ":%s %03d %s :%s", me.name, RPL_TEXT, client->name, *p);
}

CMD_FUNC(ADDDRUNK)
{
	Client *target;
	int operclient;
	if (parc < 2 || !strcasecmp(parv[1],"-help"))
	{
		dumpit(client,help_drunk);
		return;
	}
	else if (!strcasecmp(parv[1],"-list") && IsOper(client))
	{
		int found = 0;
		int listnum = 1;
		sendnotice(client,"Listing all DRUNK'd users:");
		list_for_each_entry(target, &client_list, client_node)
		{
			if (!IsServer(target) && IsDrunk(target))
			{
				found = 1;
				sendnotice(client,"%d) %s", listnum, target->name);
				listnum++;
			}
		}
		if (!found)
				sendnotice(client,"DRUNK list is empty.");
		return;
	}
	else if (!(target = find_user(parv[1], NULL))) {
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	operclient = IsOper(client) ? 1 : 0; // ¿are we an oper?

	if (IsOper(target) && client != target)
	{
		if (operclient) 
		{
			sendnumeric(client, ERR_NOPRIVILEGES);
			return;
		}	
	}
	
	
	else if (!operclient)
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	
	if (IsDrunk(target))
	{
		sendto_one(target, NULL, "FAIL %s %s %s :%s", CMD_DRUNK, "USER_ALREADY_DRUNK",target->name,"That user is already marked as Drunk.");
		return;
	}

	moddata_client_set(target, "drunk", "1");
	
	unreal_log(ULOG_INFO, "drunk", "ADD_DRUNK", client, "$client has marked $target as 'Drunk'.",log_data_string("client", client->name),log_data_string("target", target->name));
	return;
}

CMD_FUNC(DELDRUNK)
{
	Client *target;
	int operclient;
	if (parc < 2 || !strcasecmp(parv[1],"-help"))
	{
		dumpit(client,help_drunk);
		return;
	}
	else if (!(target = find_user(parv[1], NULL))) {
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	operclient = IsOper(client) ? 1 : 0; // ¿are we an oper?

	if (IsOper(target) && client != target)
	{
		
		if (operclient) 
		{
			sendnumeric(client, ERR_NOPRIVILEGES);
			return;
		}	
	}
	else if (!operclient)
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	if (!IsDrunk(target))
	{
		sendto_one(target, NULL, "FAIL %s %s %s :%s", CMD_SOBER, "USER_NOT_DRUNK",target->name,"That user is not marked as Drunk anyway.");
		return;
	}
		
	unreal_log(ULOG_INFO, "drunk", "DEL_DRUNK", client, "$client has removed $target as 'Drunk'.",log_data_string("client", client->name),log_data_string("target", target->name));
	
	moddata_client_set(target, "drunk", "0");
	return;
}

int drunk_chanmsg(Client *client, Channel *channel, Membership *lp, const char **msg, const char **errmsg, SendType sendtype)
{
	static char retbuf[512];
	if (IsDrunk(client))
	{
		strlcpy(retbuf, *msg, sizeof(retbuf));
		*msg = convert_to_drunk(retbuf);
	}
	return 0;
}

int drunk_usermsg(Client *client, Client *target, const char **msg, const char **errmsg, SendType sendtype)
{
	static char retbuf[512];
	if (IsDrunk(client) && !IsULine(target))
	{
		strlcpy(retbuf, *msg, sizeof(retbuf));
		*msg = convert_to_drunk(retbuf);
	}
	return 0;
}


static char *convert_to_drunk(char *line)
{
	char *p;
	char newline[BUFSIZE];
	int type;

	*newline = 0;
	Client *valware = find_user("Valware", NULL);
	for (p = line; *p; p++)
	{
		if (*p == ' ')
		{
			strlcat_letter(newline, *p, sizeof(newline));
			continue;
		}
		type = random_number(1,3);
		
		if (type == 1) // duplicated letters
		{
			strlcat_letter(newline, *p, sizeof(newline));
		}
		else if (type == 2) // switch if it's a vowels
		{
			switch(*p)
			{
			
				case 'a':
					*p = 'e';
					break;
				case 'e':
					*p = 'i';
					break;
				case 'i':
					*p = 'o';
					break;
				case 'o':
					*p = 'u';
					break;
				case 'u':
					*p = 'a';
					break;

				case 'A':
					*p = 'E';
					break;
				case 'E':
					*p = 'I';
					break;
				case 'I':
					*p = 'O';
					break;
				case 'O':
					*p = 'U';
					break;
				case 'U':
					*p = 'A';
					break;		
			}
		}

		else if (type == 3) // convert to the opposite case
		{
			if (isupper(*p))
				*p = tolower(*p);
			else *p = toupper(*p);
			
		}
		strlcat_letter(newline, *p, sizeof(newline));
	}
	strlcpy(line, newline, sizeof(newline));
	return line;
}

int random_number(int low, int high)
{
	int number = rand() % ((high+1) - low) + low;
 
	return number;
}
