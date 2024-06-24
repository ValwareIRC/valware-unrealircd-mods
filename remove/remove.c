/*
License: GPLv3 or later
Name: third/remove
Author: Valware
*/


/*** <<<MODULE MANAGER START>>>
module
{
		documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/remove/remove.md";
		troubleshooting "In case of problems, check the documentation or e-mail me at valerie@valware.co.uk";
		min-unrealircd-version "6.*";
		max-unrealircd-version "6.*";
		post-install-text {
				"The module is installed. Now all you need to do is add a loadmodule line:";
				"loadmodule \"third/remove\";";
				"to your configuration, and then rehash your server.";
				"See the documentation for configuration instructions.";
		}
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"

ModuleHeader MOD_HEADER
  = {
	"third/remove",
	"1.0",
	"Adds command /REMOVE",
	"Valware",
	"unrealircd-6",
};


struct configstruct
{
	int support_nokicks;
	char *protected_rank;

	int got_protected_rank;
	int got_support_nokicks;
};
static struct configstruct conf;

void setconf(void);
int remove_configtest(ConfigFile *cf, ConfigEntry *ce, int type, int *errs);
int remove_configrun(ConfigFile *cf, ConfigEntry *ce, int type);
#define REMOVE "REMOVE"
#define FPART "FPART"
	
CMD_FUNC(CMD_REMOVE);
CMD_FUNC(CMD_FPART);

MOD_TEST()
{
	memset(&conf, 0, sizeof(conf));

	HookAdd(modinfo->handle, HOOKTYPE_CONFIGTEST, 0, remove_configtest);
	return MOD_SUCCESS;
}

MOD_INIT()
{
	/*  Mark this as a global module. All this means is that UnrealIRCd will
		complain about it if you didn't load it on all servers on your
		network, because clients would not function correctly unless it were.
	*/
	MARK_AS_GLOBAL_MODULE(modinfo);

	/*  Add our commands "REMOVE" and "PART" to the IRCd to expect 
		2 parameter(s) and to be allowed to be used by regular users.
		In the command function, we will check if the user is a
		channel operator before executing this command.
	*/

	setconf();
	CommandAdd(modinfo->handle, REMOVE, CMD_REMOVE, 3, CMD_USER);
	CommandAdd(modinfo->handle, FPART, CMD_REMOVE, 3, CMD_USER);
	return MOD_SUCCESS;
}

MOD_LOAD()
{
	return MOD_SUCCESS;
}

MOD_UNLOAD()
{
	return MOD_SUCCESS;
}

/** The command functionality (What happens when a user types "/REMOVE" or "/FPART".)
	The following is boilerplate code and doesn't do anything and is there for example purposes.
	The return value is void.
*/
CMD_FUNC(CMD_REMOVE)
{
	/* Not enough parameters */
	if (parc < 3)
	{
		sendnumeric(client, ERR_NEEDMOREPARAMS, "REMOVE");
		return;
	}
	
	/* Only allow channel halfops and above to use this command */
	if (!IsUser(client))
		return;

	Channel *channel = find_channel(parv[2]);
	Client *target = find_user(parv[1], NULL);

	if (!channel)
	{
		sendnumeric(client, ERR_NOSUCHCHANNEL, parv[2]);
		return;
	}
	if (!target)
	{
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	
	if (!check_channel_access(client, channel, "hoaq"))
	{
		sendnumeric(client, ERR_CANNOTDOCOMMAND, "REMOVE", "No access");
		return;
	}

	if (conf.support_nokicks && has_channel_mode(channel, 'Q'))
	{
		sendnumeric(client, ERR_CANNOTDOCOMMAND, "REMOVE", "Channel is +Q");
		return;
	}
	if (conf.protected_rank && check_channel_access(client, channel, conf.protected_rank))
	{
		sendnumeric(client, ERR_CANNOTDOCOMMAND, "REMOVE", "User is protected");
		return;
	}

	parv[0] = target->name;
	parv[1] = channel->name;
	parv[2] = NULL;
	do_cmd(target, NULL, "PART", 2, parv);
	
}

void setconf(void)
{
	conf.support_nokicks = 0; // disabled by default
	safe_strdup(conf.protected_rank,"a");
}

int remove_configtest(ConfigFile *cf, ConfigEntry *ce, int type, int *errs)
{
	int errors = 0;
	int i;
	ConfigEntry *cep;

	if(type != CONFIG_MAIN)
		return 0;

	if(!ce || !ce->name)
		return 0;

	if(strcmp(ce->name, "remove"))
		return 0;

	for(cep = ce->items; cep; cep = cep->next)
	{
		if(!cep->name)
		{
			config_error("%s:%i: blank %s item", cep->file->filename, cep->line_number, REMOVE);
			errors++;
			continue;
		}

		if(!cep->value)
		{
			config_error("%s:%i: blank %s value", cep->file->filename, cep->line_number, REMOVE);
			errors++;
			continue;
		}

		if(!strcmp(cep->name, "support-nokicks"))
		{
			if(conf.got_support_nokicks)
			{
				config_error("%s:%i: duplicate %s::%s directive", cep->file->filename, cep->line_number, REMOVE, cep->name);
				errors++;
				continue;
			}

			conf.got_support_nokicks = 1;

			if(!strlen(cep->value))
			{
				config_error("%s:%i: %s::%s cannot be empty", cep->file->filename, cep->line_number, REMOVE, cep->name);
				errors++;
			}
			continue;
		}
		if(!strcmp(cep->name, "protected-rank"))
		{
			if(conf.got_protected_rank)
			{
				config_error("%s:%i: duplicate %s::%s directive", cep->file->filename, cep->line_number, REMOVE, cep->name);
				errors++;
				continue;
			}

			conf.got_protected_rank = 1;

			if(!strlen(cep->value))
			{
				config_error("%s:%i: %s::%s cannot be empty", cep->file->filename, cep->line_number, REMOVE, cep->name);
				errors++;
			}
			continue;
		}

		config_warn("%s:%i: unknown item %s::%s", cep->file->filename, cep->line_number, REMOVE, cep->name);
	}

	*errs = errors;
	return errors ? -1 : 1;
}
int remove_configrun(ConfigFile *cf, ConfigEntry *ce, int type)
{
	ConfigEntry *cep;

	if(type != CONFIG_MAIN)
		return 0; 

	if(!ce || !ce->name)
		return 0;

	if(strcmp(ce->name, "remove"))
		return 0;

	for(cep = ce->items; cep; cep = cep->next)
	{
		if(!cep->name)
			continue;

		if(!strcmp(cep->name, "suppport-nokicks"))
		{
			conf.support_nokicks = config_checkval(cep->value, CFG_YESNO);
			continue;
		}

		if(!strcmp(cep->name, "protected-rank"))
		{
			safe_strdup(conf.protected_rank,cep->value);
			continue;
		}
	}

	return 1; // We good
}
