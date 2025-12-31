
#include "unrealircd.h"

ModuleHeader MOD_HEADER = {
    "third/sendraw",
    "1.0",
    "Send raw lines to people and channels",
    "Valware",
    "unrealircd-6"
};

CMD_FUNC(cmd_sendraw)
{
    bool is_chan = false;
    if (BadPtr(parv[1]) || BadPtr(parv[2]))
    {
        sendnumeric(client, ERR_NEEDMOREPARAMS, "SENDRAW");
        return;
    }
    if (parv[1][0] == '#')
        is_chan = true;

    if (is_chan)
    {
        Channel *chan = find_channel(parv[1]);
        if (!chan)
        {
            sendnumeric(client, ERR_NOSUCHCHANNEL, parv[1]);
            return;
        }
        sendto_channel(chan, &me, NULL, 0, 0, SEND_LOCAL|SEND_REMOTE, NULL, ":%s", parv[2]);
    } else {
        Client *target = find_user(parv[1], NULL);
        if (!target)
        {
            sendnumeric(client, ERR_NOSUCHNICK, parv[1]);
            return;
        }
        sendto_one(target, NULL, ":%s", parv[2]);
    }
    unreal_log(ULOG_INFO, "sendraw", "SENDRAW", NULL, "$client.name sent raw line to '$target': \":$rawline\"",
         log_data_client("client", client), log_data_string("target", parv[1]), log_data_string("rawline", parv[2]));
}

MOD_INIT()
{
    CommandAdd(modinfo->handle, "SENDRAW", cmd_sendraw, 2, CMD_OPER);
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
