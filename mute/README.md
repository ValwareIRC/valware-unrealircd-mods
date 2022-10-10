## Mute ##
This module allows opers to mute users. Users will not be able to speak in private message or in channels (with the exception of Opers and your configuration-defined `help-channel`), but may still do everything else. This module can be considered something of a lesser-shun The mute will be displayed in the users `whois` to opers.

## Options ##
You can choose whether or not to display an error to the user about their lost message. You can also set what message you want them to see in the configuration. You can also lag them for a configurable time.

## Syntax ##
List all muted users
`/MUTE -list`

Display command help
`/MUTE -help`

Mute a user called Lamer32
`/MUTE Lamer32`

Unmute a user called Valware
`/UNMUTE Valware`

## Configuration ##
Here is an example configuration block displaying the defaults. If you do not have a `mute` configuration block, it will use these as defaults.
```
mute {
        show-reason yes;
        reason "You cannot send or receive messages except for staff and the support channel.";
        lag-time 500; // half a second
}

```
