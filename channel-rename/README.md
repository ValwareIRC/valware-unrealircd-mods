## Implements [IRCv3's Channel Rename](https://ircv3.net/specs/extensions/channel-rename)
This module needs to be loaded on all servers on the network.

Based on: https://ircv3.net/specs/extensions/channel-rename

Implements the ability to rename channels with the command `/RENAME`.
Syntax:
```
RENAME <old-name> <new-name> [<reason>]
```
This is implemented with strict logic, specifically that you MUST be have channel-owner mode (+q) or be an IRCOp with a `channel:rename` operclass permission in order to rename a channel.

### ⚠️ DESYNC WARNING
Right now there is not much support for this command, including in services packages, meaning:
- If you rename a channel that is registered, it will not transfer the registered channel in a meaningful way in services.
- You also likely will not be able to register a renamed channel since services will think you are not on that channel.
- Other pseudoservers such as stats severs or custom servers will desync unless channel renaming is also implemented in their software.

It is therefore not recommended to run this module yet until there is more compatibility, unless you don't use a services package, in which case go nuts.

If you wanted to test this module, I would NOT suggest doing so in a production environment since I would recommend switching off your services package and trying out renaming a few unregistered channels.

### ⚠️ OTHER WARNINGS lol
If your sever doesn't use +q, for example using the [blacklist-module directive](https://www.unrealircd.org/docs/Blacklist-module_directive) to blacklist the module "chanmodes/chanowner" (which provides +q for `~` channel users) then regular users cannot rename channels.

### Configuration
There is no required configuration but there is one optional directive to decide the cooldown for renaming a channel. Here is an example using default value:
```
channel-rename {
  allowed-interval 15m;
}
```
