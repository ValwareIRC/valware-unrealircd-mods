## CMDSLIST ##
Provides a subscription-based list of commands for clients to make a command-suggestion or autocomplete type feature for their users.
By requesting the capability, you are subscribing to **changes** and you will not automatically be sent a list. You'll need to request the first list, and any changes to the commands you can perform will be updated as it happens, for example on (de)oper

## Spec ##
This is not based on any spec and is welcome to ideas for improvement, mainly written to try and feel how useful it will be before I submit a proposal for it to IRCv3

## POC Client Code (mIRC) ##
mIRC Script can be found [here](https://github.com/ValwareIRC/mIRC-Scripts/blob/main/autocomplete/autocomplete.mrc)
## How to use ##
### What To Send ###
During connect (or at any point really) request the cap `valware.uk/cmdslist` and request the list with `CMDSLIST`
```
CAP REQ valware.uk/cmdslist`
CMDSLIST
```
### What You'll Receive ###
You'll receive a list of commands you can (or cannot) use. The format is the same whether you are requesting this list now or it's being updated later.
You can see the difference between a command which is being added and which is being removed from your ability by its prefix which is either a `+` or a `-`.

Below is an example of the command `NICK` being indicated as a command that you can use.
```
:irc.example.org CMDSLIST +NICK
```

Below is an example of the command `GLINE` being indicated as a command that you **can no longer** use.
```
:irc.example.org CMDSLIST -GLINE
```

