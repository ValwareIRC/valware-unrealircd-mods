### OJOIN ###

Inspired by InspIRCd's module of the same name

Provides PrefixMode `+Y` which uses prefix `!`.

## Config (Optional) (From v2.0 onwards)
```
ojoin {
  show-entry-message yes; // default
  entry-message "$nick has joined on official network bidness"; // example
  //show-to "@"; // show to ops and higher 
}
```

## Syntax
`OJOIN <chan>`

Example:

`OJOIN #unrealircd`


## Information
The user must have oper, and have the `ojoin` permission in their oper block.
You can unset the mode from yourself in a channel, but you cannot set it on yourself.
You must use `/OJOIN` if you want to have it in the channel.
