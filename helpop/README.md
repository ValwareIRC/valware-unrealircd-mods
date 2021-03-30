This module provides the following:
  
  - Usermode h (HelpOp)
  - Channel mode g (Only HelpOp can join)
  - Command: /helpoper   - this sends a server notice to other users who have usermode +h if the user also have +h
  (Syntax: /helpoper your message to send)
  - Command: /report     - this sends server notice from a user with no usermode +h to users who have usermode +h
  (Syntax: /report your message to send)
  - SWHOIS line marking +h "is available for help"
 
 
Shout out to Gottem for his dank templates
Shout out to Syzop for his epic documentation skillz

Parts of code taken from parts of unrealircd5.0.9 source code

This module have no configurable option

This module is a re-make of the old helpop module (which was replaced by the then /helpop ?cmds) which
was removed in UnrealIRCd v4 but I liked it so I maked it again
![alt text](https://i.redd.it/hw8z6jdu1na31.jpg)


This module needs to be installed on every server on your network
 
