# LockServ

- Lock a server; prohibit new connections to a particular server.


Note: This is not a G-Line nor a Z-Line, it's completely separate. Unlike a G-Line this will prohibit use of SASL in addition to locking the server, protecting against bruteforce attacks, yet will allow viewing the reason, unlike Z-Line-over-TLS.

### Operclass Permissions ###
Requires operclass permission `lockserv`. Alternatively you can allow someone to only lock a server but not unlock it, or the other way round. Here are some examples using the netadmin operclass:
```
operclass netadmin {
	permissions {
		/* using this will give ability to both lock and unlock */
		lockserv;
		
		/* using this is exactly the same as the above example, gives both */
		lockserv { can_lock; can_unlock; }

		/* using this will allow the oper to ONLY lock a server and not unlock it */
		lockserv { can_lock; }
		
		/* using this will allow the oper to ONLY UNlock a server and not lock it */
		lockserv { can_unlock; }
	}
}

```


### Syntax ###
```
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
```
### I've locked myself out, how do I get back in? ###
In order to get round the lock, the module will check for a z-line ban exemption. If you don't have one, you can add one in the unrealircd config like so and it will allow you in (replacing the IP with your IP of course):
```
except ban {
	mask { ip 192.168.*; }
	type { zline; }
}
```
Don't forget to `./unrealircd rehash`!
