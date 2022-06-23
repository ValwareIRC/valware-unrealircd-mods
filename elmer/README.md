# Description
Force a user to speak like Elmer;

My port of AngryWolf's module for Unreal3.2 of the same name

replaces 'r' and 'l' with 'w' on a victims output:

Syntax: /ELMER Valware

Syntax: /DELMER Valware

`<Valware> I'm hunting rabbits, be very very quiet.`

  ->
  
`<Valware> I'm hunting wabbits, be vewy vewy quiet.`
  
  
(The user will not see that they are talking like Elmer)


Requires oper

Cannot be set on other opers

![elmer](https://c.tenor.com/L9LxlrJudBEAAAAC/elmer-fudd-hunter.gif)

# Update History

### _V2.1_ ###
Date: `23rd June 2022`

Information:

- Will now display help when you don't specify the parameter, or if the parameter is `-help`
- Will now display a list of users when the parameter is `-list` (opers only).
- Removes sending the elmer command to other servers to deal with and uses [`MD`](https://www.unrealircd.org/docs/Dev:Module_Storage) instead. Less clean for uneven modules-per-server with regards to cases where an error message may not be returned when trying to `/ELMER` someone on a server which doesn't have `elmer.c` loaded, but I decided it was a bit too messy in support of incorrectly configured servers and so took it out.
- Now logs using `unreal_log()` as a confirmation to opers who set it, however as this notice wouldn't actually show up to a non-oper `/ELMER`ing themselves, so I added in a little notice just for this case.


