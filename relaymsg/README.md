## third/relaymsg
This UnrealIRCd module lets relay bots like Matterbridge relay messages using RELAYMSG.

Bot must be a channel op.

No configuration is required, but you can change the `user@host` portion of the spoofed nick to something else like so:
```
relaymsg { hostmask "weewoo@im.a.hostmask"; }
```
