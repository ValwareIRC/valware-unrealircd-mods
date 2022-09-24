#include "unrealircd.h"

ModuleHeader MOD_HEADER = {
	"third/uwu",
	"1.0",
	"Make users, channels or servers uwu",
	"Valware",
	"unrealircd-6",
};


#define IsUwu(x)			(moddata_client(x, uwu_md).i)
#define IsUwuChannel(x)	(moddata_channel(x, uwu_channel_md).i)
#define SetUwu(x)		do { moddata_client_set(x, "uwu", "1"); } while(0)
#define SetUwuChannel(x)	do { moddata_channel(x, uwu_channel_md).i = 1; uwu_channel_sync(x); } while(0)
#define ClearUwu(x)		do { moddata_client_set(x, "uwu", "0"); } while(0)
#define ClearUwuChannel(x)	do { moddata_channel(x, uwu_channel_md).i = 0; uwu_channel_sync(x); } while(0)

#define CMD_UWU "UWU"

CMD_FUNC(ADDUWU);

void uwu_free(ModData *m);
const char *uwu_serialize(ModData *m);
void uwu_unserialize(const char *str, ModData *m);
void uwu_channel_mdata_free(ModData *m);
const char *uwu_channel_serialize(ModData *m);
void uwu_channel_unserialize(const char *str, ModData *m);
void uwu_channel_sync();

static char *convert_to_uwu(char *line);
int uwu_chanmsg(Client *client, Channel *channel, Membership *lp, const char **msg, const char **errmsg, SendType sendtype);
int uwu_usermsg(Client *client, Client *target, const char **msg, const char **errmsg, SendType sendtype);
static void send_to_client_lol(Client *client, char **p);

static char *help_uwu[] = {
	"***** /UWU *****",
	"-",
	"Forces a user or channel or server to talk using uwu.",
	"-",
	"Syntax:",
	"    /UWU <-list|-help|<nick>|<#chan>|<irc.server>| * > [[on|off] | [-c|-s|-n]]",
	" ",
	"    -c = channels",
	"    -s = servers",
	"    -n = nicks/users",
	"-",
	"Examples:",
	"-",
	"List everyone and everything who are uwu'd (requires oper):",
	"    /UWU -list",
	"-",
	"List only users who are marked as uwu.",
	"    /UWU -list -n",
	"-",
	"View this output:",
	"    /UWU -help",
	"-",
	"Force a user to uwu:",
	"    /UWU Lamer32 on",
	"-",
	"Force a whole channel to uwu:",
	"    /UWU #UwuChannel on",
	"-",
	"Force a whole server to uwu:",
	"    /UWU uwu.irc.server on",
	"-",
	"Check the status of a channel:",
	"    /UWU #UwuChannel",
	"-",
	"Remove a user from uwu:",
	"    /UWU Lamer32 off",
	"-",
	NULL
};
ModDataInfo *uwu_md;
ModDataInfo *uwu_channel_md = NULL;


MOD_INIT() {
	ModDataInfo mreq;

	MARK_AS_GLOBAL_MODULE(modinfo);
	
	/* Users and Servers are considered clients so this is their MD*/
	memset(&mreq, 0, sizeof(mreq));
	mreq.name = "uwu";
	mreq.free = uwu_free;
	mreq.serialize = uwu_serialize;
	mreq.unserialize = uwu_unserialize;
	mreq.sync = MODDATA_SYNC_EARLY;
	mreq.remote_write = 1;
	mreq.type = MODDATATYPE_CLIENT;
	uwu_md = ModDataAdd(modinfo->handle, mreq);
	if (!uwu_md)
		abort();

	/* Channels MD */
	memset(&mreq, 0, sizeof(mreq));
	mreq.name = "uwu_channel";
	mreq.serialize = uwu_serialize;
	mreq.unserialize = uwu_unserialize;
	mreq.free = uwu_free;
	mreq.sync = MODDATA_SYNC_EARLY;
	mreq.remote_write = 1;
	mreq.type = MODDATATYPE_CHANNEL;
	uwu_channel_md = ModDataAdd(modinfo->handle, mreq);
	if (!uwu_channel_md)
		abort();

	CommandAdd(modinfo->handle, CMD_UWU, ADDUWU, 2, CMD_OPER);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_CHANNEL, 0, uwu_chanmsg);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_USER, 0, uwu_usermsg);
	
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

/* ModData stuff */
const char *uwu_serialize(ModData *m) // get it
{
	static char buf[32];
	if (m->i == 0)
		return NULL; /* not set */
	snprintf(buf, sizeof(buf), "%d", m->i);
	return buf;
}
void uwu_free(ModData *m) // free it
{
	m->i = 0;
}
void uwu_unserialize(const char *str, ModData *m) // put it
{
	m->i = atoi(str);
}

/* dump strings to the client */
static void send_to_client_lol(Client *client, char **p)
{
	if(IsServer(client))
		return;
	for(; *p != NULL; p++)
		sendto_one(client, NULL, ":%s %03d %s :%s", me.name, RPL_TEXT, client->name, *p);
}

/* we have to manually send the MD for channels to other servers because there seems not to be a built-in function that I can find */
void uwu_channel_sync(Channel *channel)
{
	sendto_server(NULL, 0, 0, NULL, ":%s MD channel %s uwu_channel :%d", me.id, channel->name, IsUwuChannel(channel));
}
/*--------oOo-------- Our /UWU command --------oOo--------
 * -
 * parv[1]:
 * 	Channel or User or Server or "*", or "-list" or "-help"
 *
 * parv[2]:
 *	if parv[1] was a Channel, User, or Server:
 *		"on" or "of" or empty to view the status.
 *
 *		Note: 	You can also specify an asterisk ("*") in place of a target
 *				to affect all users, everywhere.
 *				Note++:	On large networks this could cause -list'ing to
 *						result in you disconnecting from exceeding RecvQ.
 *
 *	else, if parv[1] was "-list":
 *		"-s" for listing just servers.
 *		"-n" for listing just nicks.
 *		"-c" for listing just channels.
 *
 *	Example:
 *		/UWU Lamer32 on
 *		/UWU -help
 *		/UWU -list -s
 *		/UWU * off
 *	
 *----------oOo------------------oOo------------------oOo---------
 */
CMD_FUNC(ADDUWU)
{
	if (!IsOper(client))
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;
	}

	Client *target; /* for if the target is a user or server */
	Channel *channel = NULL; /* for if the target is a channel */

	/* User asked for the help output, dump it and return */
	if (parc < 2 || !strcasecmp(parv[1],"-help"))
	{
		send_to_client_lol(client, help_uwu);
		return;
	}

	/* User requested to list */
	else if (!strcasecmp(parv[1],"-list"))
	{
		/* some ints to keep track of things */
		int found = 0, listnum = 1, findchannels = 0, findusers = 0, findservers = 0;

		/* User wants to see the entire list of Uwu subjects */
		if (!parv[2] || !strcmp(parv[2],"*") || !strcasecmp(parv[2],"-all"))
		{
			sendnotice(client,"Listing all which is Uwu:");
			findchannels++;
			findusers++;
			findservers++;
		}

		/* User only wants to list Uwu channels */
		else if (!strcasecmp(parv[2],"-c"))
		{
			sendnotice(client,"Listing all Uwu channels:");
			findchannels++;
		}

		/* User only wants to list Uwu Servers */
		else if (!strcasecmp(parv[2],"-s"))
		{
			sendnotice(client,"Listing all Uwu servers:");
			findservers++;
		}

		/* User only wants to list Uwu Users */
		else if (!strcasecmp(parv[2],"-n"))
		{
			sendnotice(client,"Listing all Uwu users:");
			findusers++;
		}

		else // invalid syntax
		{
			send_to_client_lol(client, help_uwu);
			return;
		}
		/* If we need to loop the clients */
		if (findusers)
		{
			/* loop-de-loop */
			list_for_each_entry(target, &client_list, client_node)
			{
				if (IsUwu(target)) /* If we found a uwu user and want to see them */
				{
					found = 1;
					sendnotice(client,"%d) %s", listnum, target->name); /* show them */
					listnum++;
				}
			}
		}
		if (findservers)
		{
			list_for_each_entry(target, &global_server_list, client_node)
			{
				if (IsUwu(target)) /* If we found a uwu server and want to see them */
				{
					found = 1;
					sendnotice(client,"%d) %s", listnum, target->name); /* show them */
					listnum++;
				}
			}
		}
		/* if we are looking for channels =] */
		if (findchannels)
		{
			for (channel = channels; channel; channel = channel->nextch)
			{
				if (IsUwuChannel(channel))
				{
					found = 1;
					sendnotice(client,"%d) %s", listnum, channel->name);
					listnum++;
				}
			}
		}

		/* if we didn't find anything let them know */
		if (!found)
				sendnotice(client,"Nothing Uwu here.");

		/* return after the user listed */
		return;
		
	}
	
	/* if we're affecting everyone! */
	if (!strcmp(parv[1],"*"))
	{
		/* To be sane here, when we set it "on", we are setting all servers to "on" instead of all users (imagine the list size!)
		 * However when we set it to "off", we are going to turn off channels, servers and user clients (everything basically)
		 */

		 if (!strcasecmp(parv[2],"on"))
		 {
			list_for_each_entry(target, &global_server_list, client_node)
			{
				if (!IsUwu(target))
					SetUwu(target);
			}
			sendnotice(client,"All servers are now Uwu!");
		 }
		 else if (!strcasecmp(parv[2],"off"))
		 {
			list_for_each_entry(target, &client_list, client_node)
		 	{
				if (IsUwu(target))
					ClearUwu(target);
			}
			list_for_each_entry(target, &global_server_list, client_node)
		 	{
				if (IsUwu(target))
					ClearUwu(target);
			}
			for (channel = channels; channel; channel = channel->nextch)
			{
				if (IsUwuChannel(channel))
					ClearUwuChannel(channel);
			}
			sendnotice(client,"Nobody is uwu anymore!");

		 }
		 else /* syntax error */
			 send_to_client_lol(client, help_uwu);

		return;
	}

	/* Accio target! */
	if (!(target = find_user(parv[1], NULL)))
		if (!(target = find_server(parv[1], NULL)))
			if (!(channel = find_channel(parv[1])))
			{
				/* found nothing */
				sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
				return;
			}

	/* if we didn't have an "on/off" switch just show the status of the target */
	if (BadPtr(parv[2]) || (strcasecmp(parv[2],"on") && strcasecmp(parv[2],"off")))
	{
		if (target)
		{
			if (IsUwu(target))
				sendnotice(client,"Client/Server %s is marked as Uwu", target->name);
			else
				sendnotice(client,"Client/Server %s is not marked as Uwu", target->name);
		}
		else if (channel)
		{
			if (IsUwuChannel(channel))
				sendnotice(client,"Channel %s is marked as Uwu", channel->name);
			else
				sendnotice(client,"Channel %s is not marked as Uwu", channel->name);
		}
		return;
	}

	/* if we're switching something on */
	if (!strcasecmp(parv[2],"on"))
	{
		/* if it's a user/server */
		if (target)
		{
			/* if it's already Uwu, fail it */
			if (IsUwu(target))
			{
				sendto_one(client, NULL, "FAIL %s %s %s :%s", CMD_UWU, "CLIENT_ALREADY_UWU", target->name, "That user/server is already marked as Uwu.");
				return;
			}
			/* log it */
			unreal_log(ULOG_INFO, "uwu", "ADD_UWU", client, "$client has marked $target as 'Uwu'.",log_data_string("client", client->name),log_data_string("target", target->name));

			/* set it */
			SetUwu(target);
		}

		/* if it's a channel */
		if (channel)
		{
			/* if it's already Uwu, fail it */
			if (IsUwuChannel(channel))
			{
				sendto_one(client, NULL, "FAIL %s %s %s :%s", CMD_UWU, "CHANNEL_ALREADY_UWU", channel->name, "That channel is already marked as Uwu.");
				return;
			}
			/* log it */
			unreal_log(ULOG_INFO, "uwu", "ADD_UWU", client, "$client has marked $target as 'Uwu'.",log_data_string("client", client->name),log_data_string("target", channel->name));

			/* set it */
			SetUwuChannel(channel);
		}
	}

	/* and if we're switching it off */
	else if (!strcasecmp(parv[2],"off"))
	{
		/* if it's a user/server */
		if (target)
		{
			/* if it's not Uwu, fail it */
			if (!IsUwu(target))
			{
				sendto_one(client, NULL, "FAIL %s %s %s :%s", CMD_UWU, "CLIENT_NOT_UWU", target->name, "That user/server was not marked as Uwu.");
				return;
			}
			/* log it */
			unreal_log(ULOG_INFO, "uwu", "ADD_UWU", client, "$client has unmarked $target as 'Uwu'.",log_data_string("client", client->name),log_data_string("target", target->name));

			/* unset it */
			ClearUwu(target);
		}

		/* if it's a channel */
		if (channel)
		{
			/* if it's not Uwu, fail it */
			if (!IsUwuChannel(channel))
			{
				sendto_one(client, NULL, "FAIL %s %s %s :%s", CMD_UWU, "CHANNEL_NOT_UWU", channel->name, "That channel is not marked as Uwu.");
				return;
			}
			/* log it */
			unreal_log(ULOG_INFO, "uwu", "ADD_UWU", client, "$client has unmarked $target as 'Uwu'.",log_data_string("client", client->name),log_data_string("target", channel->name));

			/* unset it */
			ClearUwuChannel(channel);
		}

	}
	return;
}

/* Our hook function for affecting channel messages */
int uwu_chanmsg(Client *client, Channel *channel, Membership *lp, const char **msg, const char **errmsg, SendType sendtype)
{
	static char retbuf[512];
	if (IsUwu(client) || IsUwuChannel(channel) || IsUwu(client->uplink))
	{
		strlcpy(retbuf, *msg, sizeof(retbuf));
		*msg = convert_to_uwu(retbuf);
	}
	return 0;
}

/* Our hook function for affecting user messages, except those to services */
int uwu_usermsg(Client *client, Client *target, const char **msg, const char **errmsg, SendType sendtype)
{
	static char retbuf[512];
	if ((IsUwu(client) || IsUwu(client->uplink)) && !IsULine(target))
	{
		strlcpy(retbuf, *msg, sizeof(retbuf));
		*msg = convert_to_uwu(retbuf);
	}
	return 0;
}

/* Do the actual conversion lol
 * This will put uwu on the end of what they said
 */
static char *convert_to_uwu(char *line)
{
	char *p, newline[BUFSIZE] = "\0";
	for (p = line; *p; p++)
		switch(*p)
		{
			case 'l':
			case 'r':
				*p = 'w';
				break;
			case 'L':
			case 'R':
				*p = 'W';
		}
	strlcpy(newline, line, BUFSIZE); // put the colour code at the beginning
	strlcat(newline, " uwu", BUFSIZE);
	strlcpy(line, newline, sizeof(newline));
	return line;
}
