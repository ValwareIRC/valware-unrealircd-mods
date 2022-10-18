# DEFCON for UnrealIRCd #

## Syntax ##

`/DEFCON [servername|-all] N`

N represents a number between 0 and 4.
Level | Description | Level in stars
-|-|-
0 | Off, no emergency | ☆☆☆☆
1 | Critical emergency | ★★★★
2 | High level emergency | ☆★★★
3 | Medium level emergency | ☆☆★★
4 | Low level emergency | ☆☆☆★

## Operclass Permission ##

Requires operclass permission `defcon` to use.

## Configuration ##

A `defcon` configuration block is needed for this to work.
### Example: ###

```
defcon {

	/* Default behaviour
	 * Can be "local" or "global"
	 * For example, when set to "global", using '/DEFCON 1' will apply to all servers instead of just the
	 * server it was issued from.
	*/ 
	default "local";

	/* Settings for DEFCON 1
	1 {
		/* Global message to send to all affected users when this DEFCON level
		 * is activated. Must be defined if it exists. Comment it out/delete it
		 * if you want there to be no message displayed.
		*/
		global-msg "This server is now in DEFCON level 1. Please wait. Service will resume shortly";
		
		/* The list of commands to restrict while in defcon mode 1
		 * using "all" will restrict all commands except for CAP and PING
		*/
		restrict-commands "all";
		
		/* Whether to prevent users from private messaging eachother
		 * Should be yes or no, or commented out
		*/
		restrict-privmsg yes;
		
		/* Whether to prevent users from messaging channels
		 * Should be yes or no, or commented out
		*/
		restrict-chanmsg yes;
		
		/* Whether we should prevent new connections to the server/network
		 * Should be yes or no, or commented out
		*/
		restrict-connections yes;
		
		/* Whether to prevent the creation of new channels.
		 * Note: This is not needed if you have "join" in your restrict-commands for this level
		*/
		restrict-new-chans no;
		
		/* Which usermode to set on all affected users
		 * Note: This will remove the mode when defcon is turned off,
		 * with the exception of modes listed in set::default-modes
		*/
		set-user-mode "p";
		
		/* Which channel mode to set in all channels and any newly created channels */
		set-channel-mode "I";
		
		/* Whether to kill normal (non-oper, non-uline) users who are not in any channels
		 * Should be yes or no, or commented out
		*/
		kill-hidden-users yes;
	}
	2 {
		global-msg "This server is now in DEFCON level 2. Please wait. Service will resume shortly";
		restrict-commands "topic,mode,who,nick,motd";
		restrict-privmsg yes;
		restrict-chanmsg no;
		restrict-connections yes;
		restrict-new-chans yes;
		//set-user-mode "p"; // not used so commented out
		set-channel-mode "I";
		kill-hidden-users yes;
	}
	3 {
		global-msg "This server is now in DEFCON level 3. Please wait. Service will resume shortly";
		restrict-commands "topic,mode";
		restrict-privmsg no;
		restrict-chanmsg no;
		restrict-connections yes;
		restrict-new-chans yes;
		//set-user-mode "p"; // not used so commented out
		//set-channel-mode "I"; // not used so commented out
		kill-hidden-users yes;
	}
	4 {
		global-msg "This server is now in DEFCON level 4. Please wait. Service will resume shortly";
		//restrict-commands ""; // not used
		restrict-privmsg no;
		restrict-chanmsg no;
		restrict-connections yes;
		restrict-new-chans no;
		//set-user-mode "p"; // not used
		//set-channel-mode "I"; //not used
		kill-hidden-users yes;
	}
}

```

### Description ###
Item name | Description | Expected value | Required
----------|-------------|----------------|---------
`defcon::default` | Whether to action by local server or globally by default | `"local"` or `"global"` | Yes
`defcon::N::global-msg` | A message to display to users when that DEFCON level is activated | `"string"` | No
`defcon::N::restrict-commands` | A comma-delimited list of commands to restrict | `"command1, command2"` | No
`defcon::N::restrict-privmsg` | Restrict users from sending/receiving private messages | `yes` or `no` | No
`defcon::N::restrict-chanmsg` | Restrict users from sending/receiving channel messages | `yes` or `no` | No
`defcon::N::restrict-connections` | Restrict new connections to the server/network | `yes` or `no` | No
`defcon::N::restrict-new-chans` | Restrict channels from being joined if the channel doesn't already exist | `yes` or `no` | No

Note: While most of the items are not required, numbered blocks ARE required, even if they are empty.

Example of a valid `defcon` cofiguration which does absolutely nothing:
```
defcon {
	default "local";
	1 { }
	2 { }
	3 { }
	4 { }
}
```
