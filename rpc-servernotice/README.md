# RPC Server Notice
## Description
Allows RPC users to send server notices (or privmsgs) to specific users

## How to use
### Method name
`servernotice.send`

### Params
|Param|Description|Expectation|
|-|-|-|
|`nick`|Required. The nick or UID of the user.|string|
|`msg`|Required. The message to send the user.|string|
|`as_privmsg`|Optional. Whether to send as a PRIVMSG instead of a notice.|boolean|

## Example
```json
{
  "jsonrpc":"2.0",
  "id":"123",
  "method":"servernotice.send",
  "params":{
    "nick":"Valware",
    "msg":"Yo wassup bro ima server lmao",
    "as_privmsg":false
  }
}
```
