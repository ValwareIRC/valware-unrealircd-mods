/* 

	SAYEET command for forcing someone to cycle a channel 

	Largely taken from unrealircd sauce code

*/
/*** <<<MODULE MANAGER START>>>
module
{
        documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/sayeet/README.md";
	troubleshooting "In case of problems, documentation or e-mail me at v.a.pond@outlook.com";
        min-unrealircd-version "5.*";
        max-unrealircd-version "5.*";
        post-install-text {
                "The module is installed. Now all you need to do is add a loadmodule line:";
                "loadmodule \"third/sayeet\";";
                "And /REHASH the IRCd.";
                "The module does not need any other configuration.";
        }
}
*** <<<MODULE MANAGER END>>>
*/
#include "unrealircd.h"


#define CheckAPIError(apistr, apiobj) \
	do { \
		if(!(apiobj)) { \
			config_error("A critical error occurred on %s for %s: %s", (apistr), MOD_HEADER.name, ModuleGetErrorStr(modinfo->handle)); \
			return MOD_FAILED; \
		} \
	} while(0)


/* the cmd lmao */
#define CMD "SAYEET"

CMD_FUNC(yeetus);

ModuleHeader MOD_HEADER = {
	"third/sayeet",
	"1.0",
	"\"Force someone to part and rejoin a channel",
	"Valware",
	"unrealircd-5",
};
MOD_INIT()
{
	CheckAPIError("CommandAdd(CMD)", CommandAdd(modinfo->handle, CMD, yeetus, 2, CMD_USER|CMD_SERVER));
	MARK_AS_GLOBAL_MODULE(modinfo);
	return MOD_SUCCESS;
}
MOD_LOAD(){ return MOD_SUCCESS; }
MOD_UNLOAD(){ return MOD_SUCCESS; }
CMD_FUNC(yeetus){
	Client *target;
	Channel *channel;
	Membership *lp;
	char *name, *p = NULL;
	int i;
	char *comment = (parc > 3 && parv[3] ? parv[3] : NULL);
	char commentx[512];
	char jbuf[BUFSIZE];
	int ntargets = 0;
	int maxtargets = max_targets_for_command("SAYEET");

	if ((parc < 3) || BadPtr(parv[2]))
        {
                sendnumeric(client, ERR_NEEDMOREPARAMS, "SAYEET");
                return;
        }

        if (!(target = find_person(parv[1], NULL)))
        {
                sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
                return;
        }

	/* See if we can operate on this vicim/this command */
	if (!ValidatePermissionsForPath("sacmd:sapart",client,target,NULL,NULL))
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;
	}

	/* Relay it on, if it's not my target */
	if (!MyUser(target))
	{
		if (comment)
		{
			sendto_one(target, NULL, ":%s SAYEET %s %s :%s", client->id, target->id, parv[2], comment);
			ircd_log(LOG_SACMDS,"SAYEET: %s used SAYEET to make %s part %s (%s)",
			         client->name, target->name, parv[2], comment);
		}
		else
		{
			sendto_one(target, NULL, ":%s SAYEET %s %s", client->id, target->id, parv[2]);
			ircd_log(LOG_SACMDS,"SAYEET: %s used SAYEET to make %s part %s",
			         client->name, target->name, parv[2]);
		}
		return;
	}

	/* Now works like cmd_join */
	*jbuf = 0;
	for (i = 0, name = strtoken(&p, parv[2], ","); name; name = strtoken(&p, NULL, ","))
	{
		if (++ntargets > maxtargets)
		{
			sendnumeric(client, ERR_TOOMANYTARGETS, name, maxtargets, "SAYEET");
			break;
		}
		if (!(channel = get_channel(target, name, 0)))
		{
			sendnumeric(client, ERR_NOSUCHCHANNEL,
				name);
			continue;
		}

		/* Validate oper can do this on chan/victim */
		if (!IsULine(client) && !ValidatePermissionsForPath("sacmd:sapart",client,target,channel,NULL))
		{
			sendnumeric(client, ERR_NOPRIVILEGES);
			continue;
		}

		if (!(lp = find_membership_link(target->user->channel, channel)))
		{
			sendnumeric(client, ERR_USERNOTINCHANNEL, target->name, name);
			continue;
		}
		if (*jbuf)
			strlcat(jbuf, ",", sizeof jbuf);
		strlncat(jbuf, name, sizeof jbuf, sizeof(jbuf) - i - 1);
		i += strlen(name) + 1;
	}

	if (!*jbuf)
		return;

	strcpy(parv[2], jbuf);

	if (comment)
	{
		strcpy(commentx, "SAYEET: ");
		strlcat(commentx, comment, 512);
	}

	parv[0] = target->name; // nick
	parv[1] = parv[2]; // chan
	parv[2] = NULL; // comment
	if (comment)
	{
		sendnotice(target, "*** You were forced to cycle %s (%s)", parv[1], commentx);
		sendto_umode_global(UMODE_OPER, "%s used SAYEET to make %s cycle %s (%s)",
				    client->name, target->name, parv[1], comment);
		ircd_log(LOG_SACMDS,"SAYEET: %s used SAYEET to make %s cycle %s (%s)",
			client->name, target->name, parv[1], comment);
	}
	else
	{
		sendnotice(target, "*** You were forced to cycle %s", parv[1]);
		sendto_umode_global(UMODE_OPER, "%s used SAYEET to make %s cycle %s",
				    client->name, target->name, parv[1]);
		ircd_log(LOG_SACMDS,"SAYEET: %s used SAYEET to make %s cycle %s",
			client->name, target->name, parv[1]);
	}
	do_cmd(target, NULL, "CYCLE", 2, parv);
	/* target may be killed now due to the part reason @ spamfilter */
}
