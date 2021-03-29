
#include "unrealircd.h"

#define KYSLOL "AYYY"

CMD_FUNC(ayyy);

ModuleHeader MOD_HEADER = {
	"third/ayyy", 
	"1.0",
	"may as well kys lmao", 
	"Valware",
	"unrealircd-5",
};

MOD_INIT() {
	CommandAdd(modinfo->handle, KYSLOL, ayyy, MAXPARA, CMD_USER);
	MARK_AS_GLOBAL_MODULE(modinfo);
	return MOD_SUCCESS;
}

MOD_LOAD() {
	return MOD_SUCCESS;
}


MOD_UNLOAD() {
	return MOD_SUCCESS;
}

CMD_FUNC(ayyy) {
	exit_client(client, recv_mtags, "killed myself (lmao)");
}
