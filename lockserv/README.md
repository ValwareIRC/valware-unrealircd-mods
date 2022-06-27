# LockServ

Lock a server; prohibit new connections to a particular server.

Note: Unlike a G-Line this will prohibit use of SASL in addition to locking the server, but will allow viewing the reason, unlike Z-Line.

Requires operclass permission `lockserv`.

Can be split up into `lockserv { can_lock; can_unlock; }`

```
Syntax:
	/LOCKSERV <server|nick|-list> [<reason>]
	/UNLOCKSERV <server>

Examples:

List locked servers:
	/LOCKSERV -list

View this output:
	/LOCKSERV -help

Lock a server called 'lol.valware.uk':
	/LOCKSERV lol.valware.uk This server is closed! Please connect to irc.valware.uk instead

Lock the server that user 'Valware' is on:
	/LOCKSERV Valware Server closed for spam.

Unlock a server to resume allowing connections:
	/UNLOCKSERV Lamer32
