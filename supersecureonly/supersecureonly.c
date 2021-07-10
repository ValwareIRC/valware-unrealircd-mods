/*
  Licence: GPLv3
*/
/*** <<<MODULE MANAGER START>>>
module
{
        documentation "https://github.com/ValwareIRC/valware-unrealircd-mods/blob/main/webirconly/README.md";
		troubleshooting "In case of problems e-mail me at v.a.pond@outlook.com";
        min-unrealircd-version "5.*";
        max-unrealircd-version "5.*";
        post-install-text {
                "The module is installed. Now all you need to do is add a loadmodule line:";
                "loadmodule \"third/supersecure\";";
                "And /REHASH the IRCd.";
                "The module does not need any other configuration.";
        }
}
*** <<<MODULE MANAGER END>>>
*/
#include "unrealircd.h"

Cmode_t SUPERSECURE_ONLY;

#define SUPERSECURE_FLAG '1'

#define IsWebircOnly(channel)    (channel->mode.extmode & SUPERSECURE_ONLY)
#define IsWebircUser(x) (IsUser(x) && supersecurechecklol(x))

#define CheckAPIError(apistr, apiobj) \
	do { \
		if(!(apiobj)) { \
			config_error("A critical error occurred on %s for %s: %s", (apistr), MOD_HEADER.name, ModuleGetErrorStr(modinfo->handle)); \
			return MOD_FAILED; \
		} \
	} while(0)



static int supersecurechecklol(Client *cptr);

ModuleHeader MOD_HEADER = {
	"third/supersecureonly", // Module name
	"1.0", // Module Version
	"Super secure channel", // Description
	"Valware", // Author
	"unrealircd-5", // Unreal Version
};

int supersecureonly_check (Client *client, Channel *channel, char *key, char *parv[]);


// Initialisation routine (register hooks, commands and modes or create structs etc)
MOD_INIT() {
	CmodeInfo req;

	memset(&req, 0, sizeof(req));
	req.paracount = 0;
	req.flag = SUPERSECURE_FLAG;
	req.is_ok = extcmode_default_requirehalfop;
	CheckAPIError("CmodeAdd(SUPERSECURE_ONLY)",CmodeAdd(modinfo->handle, req, &SUPERSECURE_ONLY));
	HookAdd(modinfo->handle, HOOKTYPE_CAN_JOIN, 0, supersecureonly_check);
	

	MARK_AS_GLOBAL_MODULE(modinfo);
	return MOD_SUCCESS;
}


MOD_LOAD() {
	return MOD_SUCCESS;
}

MOD_UNLOAD() {
	return MOD_SUCCESS;
}
int supersecureonly_check (Client *client, Channel *channel, char *key, char *parv[])
{
	
	// if it's not a webirc channel, it's not for us, return 0
	if (!IsWebircOnly(channel)){ return 0; }
	
	// if the user is an oper, let them through, return 0
	if (IsOper(client)){ return 0; }
	
	// let them through if they're logged in
	if (IsLoggedIn(client)){
		return 0;
	}
	// let them through if they're secure
	if (client->umodes & UMODE_SECURE){
		return 0;
	}
	
	// if they're not a webirc user, fuck them off
	if (IsWebircUser(client)){
		return 0;
	}
	
	// else fuck them off
	else {
		sendnumeric(client, ERR_FORBIDDENCHANNEL, channel->chname, "This channel is for registered users, web users, secure clients only.");
		return ERR_NEEDREGGEDNICK;
	}
}

// copied and edited from third/showwwebirc by k4be lol
static int supersecurechecklol(Client *cptr) {
	ModDataInfo *moddata;
	moddata = findmoddata_byname("webirc", MODDATATYPE_CLIENT);
	if(moddata == NULL) { return 0; }
	if(moddata_client(cptr, moddata).l){ return 1; }
	return 0;
}

