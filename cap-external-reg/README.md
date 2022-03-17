This module provides prototype CAP `'draft/external-reg'`

This CAP is used for letting a client know that the registration mechanism is external to IRC, and providing a link to it.

This parameters in this CAP *MUST* be set by the server/service which authenticates SASL requests.


Usage (Services only):

`MD client <sid> externalreglink :<link>`
