# MessageTag ExtBan
Enables channel operators to set restrictions and exceptions on which client-only IRCv3 message-tags can be used per channel.

This does *not* allow you to prevent server-provided tags like `time`, `label` and `msgid`.

### Group ###
This is a **Group 4** extban which means you may **only** stack it with the `~time:` extban. You may not stack with other extbans.

### Syntax ###
```
// ban all mtags and only allow typing
MODE #channel +b ~mtag:+*

// except typing
MODE #channel +e ~mtag:+typing

// hmm actually I only want to have ratified mtags and stop "draft/"-prefixed tags
MODE #channel +b ~mtag:+draft/*

// except for draft/typing for backwards compat
MODE #channel +e ~mtag:draft/typing
```
