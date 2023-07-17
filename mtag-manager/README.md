## Message-Tags Manager (`third/mtag-manager`) ##
This module allows server owners to allow whatever client-to-client message-tags that they like via the configuration.

### _A word of caution about validation_ ##
This module provides absolutely no validation for any values (given or not given) sent using tags provided by this module. This means there can be no telling what people will send as a value. The `+draft/react` tag is particular in this caution because there would be no validation or backwards-compatibility implemented and so people would be able to react using URLs - additionally, reacts can be used for bullying people and, with no backwards compatibilty, people can be hateful about someone amongst themselves without any knowledge of the user.

So for tags where this may present a problem, like `draft/react`, it's recommended that you use a module which implements it properly and safely where possible.

## Configuration ##
Tags can be specified in a `message-tags {}` block.

Client tags MUST begin with a `+` sign.

Syntax:
```
message-tags {
  <tag1>;
  <tag2>;
  <tag3>;
}
```

Here is an example of a configuration block which adds the same tags that the `third/kiwiirc-tags` module provides.
```
message-tags {
  +kiwiirc.com/fileuploader;
  +kiwiirc.com/ttt;
  +kiwiirc.com/conference;
  +data; // will be deprecated soon
}
```
