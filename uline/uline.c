#include "unrealircd.h"

ModuleHeader MOD_HEADER = {
    "third/uline",
    "2.0",
    "Command /uline",
    "Valware",
    "unrealircd-6"
};

MOD_LOAD()
{
    return MOD_SUCCESS;
}

MOD_UNLOAD()
{
    return MOD_SUCCESS;
}

/* From porting the previous source, I assume the syntax is like this:
 * /uline [+|-]nick
 * if neither of + or - is specified, + is assumed
*/
CMD_FUNC(cmd_uline)
{
    bool add = true;
    char *nick = NULL;
    if (BadPtr(parv[1]))
    {
        sendnumeric(client, ERR_NEEDMOREPARAMS, "ULINE");
        return;
    }
    
    safe_strdup(nick, parv[1]);
    if (*nick == '+')
        nick++;

    else if (*nick == '-')
    {
        nick++;
        add = 0;
    }
    // if what we're left with is empty, NULL it
    if (!*nick)
        nick = NULL;

    if (!nick)
    {
        sendnumeric(client, ERR_CANNOTDOCOMMAND, "ULINE", "Invalid params");
        return;
    }

    Client *target = find_user(nick, NULL);
    if (!target)
    {
        sendnumeric(client, ERR_NOSUCHNICK, nick);
        return;
    }

    /* Now for the cool part */
    if (add)
    {
        if (IsULine(target))
        {
            sendnotice(client, "*** %s is already U-Lined", target->name);
            return;
        }
        SetULine(target);
    } else {
        if (!IsULine(target))
        {
            sendnotice(client, "*** %s is not U-Lined", target->name);
            return;
        }

        ClearULine(target);
    }        
    
    unreal_log(ULOG_INFO, "uline",add?"ULINE_ADDED_VIA_MODULE":"ULINE_REMOVED_VIA_MODULE", NULL, "$client.name has $add an u-line: $target.name", log_data_client("client", client), log_data_client("target", target), log_data_string("add",add?"added":"removed"));

}

MOD_INIT()
{
    CommandAdd(modinfo->handle, "ULINE", cmd_uline, 1, CMD_OPER);
    return MOD_SUCCESS;
}
