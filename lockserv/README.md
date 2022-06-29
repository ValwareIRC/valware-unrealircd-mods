# LockServ

- Lock a server; prohibit new connections to a particular server.


Note: This is not a G-Line nor a Z-Line, it's completely separate. Unlike a G-Line this will prohibit use of SASL in addition to locking the server, protecting against bruteforce attacks, yet will allow viewing the reason, unlike Z-Line-over-TLS. In order to circumvent the prohibition (connect to the server while it's locked), you can give yourself a z-line ban exemption in the unrealircd config like so and it will allow you in (replacing the IP with your IP of course):
```
except ban {
	mask { ip 192.168.*; }
	type { zline; }
}
```

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
	/UNLOCKSERV lol.valware.uk
