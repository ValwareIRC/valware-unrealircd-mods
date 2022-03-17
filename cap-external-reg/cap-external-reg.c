/*
  This module provides new CAP 'draft/external-reg'

  License: GPLv3

  Copyright Ⓒ 2022 Valerie Pond
*/
/*** <<<MODULE MANAGER START>>>
module {
	documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/cap-external-reg/README.md";
	troubleshooting "In case of problems, check the README or e-mail me at v.a.pond@outlook.com";
	min-unrealircd-version "6.*";
	
	post-install-text {
		"The module is installed, now all you need to do is add a 'loadmodule' line to your config file:";
		"loadmodule \"third/cap-external-reg\";";
		"Then /rehash the IRCd.";
		"This module have no configuration";
	}
}
*** <<<MODULE MANAGER END>>>
*/

#include "unrealircd.h"

ModuleHeader MOD_HEADER
= {
	"third/cap-external-reg",
	"1.0",
	"Prototype external register CAP", 
	"Valware",
	"unrealircd-6",
};

/* Forward declarations */
void external_reg_free(ModData *m);
const char *external_reg_serialize(ModData *m);
void external_reg_unserialize(const char *str, ModData *m);
const char *external_reg_param(Client *client);

/* Variables */
long CAP_EXTREG = 0L;


int extreg_capability_visible(Client *client)
{
	if (!external_reg_param(client))
		return 0;
	return 1;
}


MOD_INIT()
{
	ClientCapabilityInfo cap;
	ModDataInfo mreq;

	MARK_AS_GLOBAL_MODULE(modinfo);

	memset(&cap, 0, sizeof(cap));
	cap.name = "draft/external-reg";
	cap.visible = extreg_capability_visible;
	cap.parameter = external_reg_param;
	ClientCapabilityAdd(modinfo->handle, &cap, &CAP_EXTREG);

	memset(&mreq, 0, sizeof(mreq));
	mreq.name = "externalreglink";
	mreq.free = external_reg_free;
	mreq.serialize = external_reg_serialize;
	mreq.unserialize = external_reg_unserialize;
	mreq.sync = MODDATA_SYNC_EARLY;
	mreq.self_write = 1;
	mreq.type = MODDATATYPE_CLIENT;
	ModDataAdd(modinfo->handle, mreq);

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

void external_reg_free(ModData *m)
{
	safe_free(m->str);
}

const char *external_reg_serialize(ModData *m)
{
	if (!m->str)
		return NULL;
	return m->str;
}

void external_reg_unserialize(const char *str, ModData *m)
{
	safe_strdup(m->str, str);
}

const char *external_reg_param(Client *client)
{
	Client *server;

	/* Check our sasl server for this cap */
	if (SASL_SERVER)
	{
		server = find_server(SASL_SERVER, NULL);
		if (server)
			return moddata_client_get(server, "externalreglink"); /* NOTE: could still return NULL */
	}

	return NULL;
}
