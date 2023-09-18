## IRCv3 React


This allows server admins to specify which reacts are allowed on their server.
This is backwards compatible, in the sense that if you don't have the specific capability,
you will see that the did they equivalent of a `/me reacts with "xyz"`

## Configuration

Admins can specify emojis or strings of text (lol, lmao, roflcopter) in the `set::allowed-reacts` directive.

Here is an example configuration that will let people use the same reacts which are used on github.
```
set { allowed-reacts "👍,👎,😃,🎉,😕,❤️,🚀,👀"; }
```

## Enabling the capability
You must enable the capability to react in your client:
```
/CAP REQ valware.uk/react
```

You can get a list of available reacts to show in your client using the command `/REACTSLIST` and you will receive a comma-delimited list of strings/emoji available for use on the server.

All reacts which don't align wih `set::allowed-reacts` will be dropped.