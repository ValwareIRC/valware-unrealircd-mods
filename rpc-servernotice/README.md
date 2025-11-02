# RPC Server Notice
## Description
Allows RPC users to send server notices (or privmsgs) to specific users

## How to use
### Method name
`servernotice.send`

### Params
|Param|Description|Type|Required|
|-|-|-|-|
|`nick`|The nick or UID of the user.|string|✅|
|`msg`|The message to send the user.<br>Can contain multiple lines using `\\n`.|string|✅|
|`as_privmsg`|Whether to send as a PRIVMSG<br>instead of a notice.|boolean|❌|

## Example
This example shows sending multiple lines, with `\\n` as the new line (remember to use double backslash!)

Of course, if you do not wish to send multiple lines, you do not need to use `\\n`
```json
{
  "jsonrpc":"2.0",
  "id":"123",
  "method":"servernotice.send",
  "params":{
    "nick":"Valware",
    "msg":"Yo wassup bro ima server lmao\\nJust kidding, welcome to the server, main channel is #lobby lol",
    "as_privmsg":false
  }
}
```
