/*
  Licence: GPLv3
  Copyright Ⓒ 2022 Valerie Pond
  Elmer
  Haz que la gente hable raro.
*/

#include "unrealircd.h"

ModuleHeader MOD_HEADER = {
	"third/elmer-es",
	"1.0",
	"Haz que la gente hable raro.",
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
	
	CommandAdd(modinfo->handle, ADDELM, ADDELMER, 1, CMD_OPER);
	CommandAdd(modinfo->handle, DELELM, DELELMER, 1, CMD_OPER);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_CHANNEL, 0, elmer_chanmsg);
	HookAdd(modinfo->handle, HOOKTYPE_CAN_SEND_TO_USER, 0, elmer_usermsg);
	
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
CMD_FUNC(ADDELMER)
{
	Client *target;
	
	if (hunt_server(client, NULL, "ELMER", 1, parc, parv) != HUNTED_ISME)
		return;
	
	if (!IsOper(client))
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	else if (parc < 2) {
		sendnumeric(client, ERR_NEEDMOREPARAMS, ADDELM);
		return;
	}
	else if (!(target = find_user(parv[1], NULL))) {
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	if (IsOper(target) && client != target)
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	if (IsElmer(target))
	{
		sendnotice(client,"%s ya estaba hablando raro!",target->name);
		return;
	}
	SetElmer(target);
	sendnotice(client,"%s ahora está hablando extraño.",target->name);
	return;
}

CMD_FUNC(DELELMER)
{
	Client *target;
	
	if (hunt_server(client, NULL, "DELMER", 1, parc, parv) != HUNTED_ISME)
		return;

	if (!IsOper(client))
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	else if (parc < 2) {
		sendnumeric(client, ERR_NEEDMOREPARAMS, DELELM);
		return;
	}
	else if (!(target = find_user(parv[1], NULL))) {
		sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
		return;
	}
	if (IsOper(target) && client != target)
	{
		sendnumeric(client, ERR_NOPRIVILEGES);
		return;	
	}
	
	if (!IsElmer(target))
	{
		sendnotice(client,"%s no estaba hablando de forma extraña de todos modos.",target->name);
		return;
	}
	ClearElmer(target);
	sendnotice(client,"%s ya no habla raro.",target->name);
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
	if (IsElmer(client))
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
