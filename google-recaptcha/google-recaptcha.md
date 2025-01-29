# Google reCAPTCHA UnrealIRCd Module v0.9 [![BETA](https://img.shields.io/badge/-BETA-red.svg)]()
## Description
Protect your UnrealIRCd network behind a beautiful and clean integration with Google reCAPTCHA.<br>
When a user is connecting to your server and they match the criteria you specified, they will be greeted with a link to a verification page covered with Google reCAPTCHA v3.<br>
When they follow this link and click the button (and whatever else form elements you specify), their connection to IRC will be automatically continued (no need for pasting codes into IRC).

```
[08:19:44] -irc.example.com- VPS users from OVH are required to prove humanity.
[08:19:44] -irc.example.com- https://example.com/verify/?t=jrzm3Nwm3XTH8UKq
```

This package comes in two parts:
- UnrealIRCd Module (this)
- Webpage you can put anywhere (installed separately)

## Requirements (including webpage requirements)
- _**UnrealIRCd 6.1.8 or higher**_ with RPC configured
- A webserver (apache2/nginx)
- PHP
- Composer
- A Google account and reCAPTCHA keys ([see link](https://www.google.com/recaptcha/admin/create))

## How to use
1. Install UnrealIRCd module:<br>```./unrealircd module install third/google-recaptcha```
2. Install the required webpage to your preferred location ([see link](https://github.com/ValwareIRC/unrealircd-google-recaptcha))
3. Configure your `unrealircd.conf` to include the module settings to point to your webpage (see below)
4. Rehash your IRCd (`./unrealircd rehash` from your terminal or `/rehash` from IRC)
5. Set an RLINE to match a mask. Supports [Extended Server Bans](https://www.unrealircd.org/docs/Extended_server_bans).

## Configuration
### Required
The minimal configuration _required_ for this module to work is as follows:
- `url` -  The URL for the verification webpage from Step 2
```
recaptcha {
  url 'https://example.com/verify/'; // Remember to use single-quotes here
}
```
### Optional
There are two other _optional_ configuration settings:
- `ipcache` - This is a time value setting between `0` and `99d` (e.g `12h` or `4w`) and determines how long to remember an IP after they were successful in completing reCAPTCHA. Users from this IP won't have to complete a reCAPTCHA when they reconnect. Set this to 0 if you don't want to cache.
- `timeout` - This is a time value setting between `30s` and `5m` (e.g `1m` or `2m30s`) and determines how long a user will have to complete the reCAPTCHA. After this time, if the user has not fully connected, the connection will terminate with the reason "Google reCAPTCHA required to connect".

Complete example using the default values:
```
recaptcha {
  url 'https://example.org/verify/';
  ipcache 1w; // 1 week
  timeout 1m; // 1 minute
}
```

## RLINE / UNRLINE
Syntax (note: using a nick resolves to their `*@iphost`):
```
RLINE <mask|nick> <duration> <reason>
UNRLINE <mask|nick>
```
`RLINE` (reCAPTCHA-Line) and `UNRLINE` a TKL-Like command which lets you define which connections are required to complete a reCAPTCHA. The reason you specify here will be the message displayed to the user upon connect.<br>
This command supports [Extended Server Bans](https://www.unrealircd.org/docs/Extended_server_bans). It is recommended that you combine this with [Security Groups](https://www.unrealircd.org/docs/Security-group_block) ([video tutorial](https://www.youtube.com/watch?v=u7vh37vEP5U)). Here are some examples:
```
/RLINE ~security-group:unknown-users 0 New users are required to prove their humanity.
/RLINE ~asn:16276 4w VPS users from OVH are required to prove humanity.
/RLINE ~account:0 0 Guests are required to prove humanity.
/RLINE *@8.8.1.1 0 Google Bots must complete Google reCAPTCHA lmao.
/RLINE Valware 24h Oops, you look like a bot, please verify your humanity.

/UNRLINE ~account:0
/UNRLINE *@8.8.1.1
```
