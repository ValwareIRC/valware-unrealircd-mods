# Account Registration (Spec is WIP)

This module provides account registration functionality for [UnrealIRCd](https://unrealircd.org).

For commands and syntax, please refer to the [IRCv3 spec for account registration](https://ircv3.net/specs/extensions/account-registration)

Please note: If you wish to use this module, you must first remove the alias for 'register' in conf/aliases.conf

# Known Implemenations
- [ ] DALEK IRC Services (ETA June)


## Server2Server

The server-to-server command is called `REGISTRATION` and follows this syntax:

`:<source> REGISTRATION <destination> <UID> <IP> <X> <Y> <Data 1> <Data 2> :<Data 3 which may contain whitespaces>`

<br>

The `<X>` token can be either one of:

`R`, which refers to registration.

`V` which refers to verification.

<br>

The `<Y>` token can be either one of:

`R` which refers to user registration. A user will send this if they are attempting to register or verify.

`A` which means 'Abort'.

`F` which means 'Fail'.

`S` which means 'Success'.

`W` which means 'Warn'.

`N` which means 'Note'.

<br>

The `<Data X>` tokens differ depending on who is sending what. For example if it's coming from a user using `R R` then:

`<Data 1>` refers to the requested account name.

`<Data 2>` refers to the email address used for registration.

`<Data 3>` refers to the users password which may contain whitespaces.

<br>

Or if the user is trying to verify a code:

`<Data 1>` refers to the account name.

`<Data 2>` in verify, is always an asterisk (*)

`<Data 3>` is the verification code.

<br>

Now, if it were the other way round and your registration mechanism is responding:

`<Data 1>` refers to the `<code>` parameter of the [Standard Reply spec](https://ircv3.net/specs/extensions/standard-replies).

`<Data 2>` refers to the `<context`> parameter of the [Standard Reply spec](https://ircv3.net/specs/extensions/standard-replies).

`<Data 3>` refers to the human-readable `<description>` parameter of the [Standard Reply spec](https://ircv3.net/specs/extensions/standard-replies).

<br>

### Visual example of registration

`:test.valware.uk REGISTRATION services.valware.uk 999VX6V02 192.168.0.20 R R Valware v.a.pond@outlook.com :This is a super secret pssword`

`:services.valware.uk REGISTRATION test.valware.uk 999VX6V02 192.168.0.20 R S * Valware :You have successfully registered an account.`


### Visual example of failed registration

`:test.valware.uk REGISTRATION services.valware.uk 999VX6V02 192.168.0.20 R R Valware email@com :This is a super secret pssword`

`:services.valware.uk REGISTRATION test.valware.uk 999VX6V02 192.168.0.20 R F INVALID_EMAIL * :The email address you've provided is invalid.`
