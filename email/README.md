# `third/smtp-email` SMTP Email module for UnrealIRCd
Send emails about the logs that you need to know about, even when you're away from IRC.

This module requires CURL.

## Expected configuration
```
email {
  host 'mail.smtp2go.com'; // Change this to your email provider's SMTPS (SSL/TLS) server
  port 465; // Change this to the right port
  username 'username@example.com'; // this is also the "from" email
  password 'yourSecretPassword000';

  // who to email?
  recipients {
    you@youremail.com;
    staff@yourserver.com;
    etc@etc.etc;
  }

  /* the Event ID of the log that you wish to be notified about. See
   * https://www.unrealircd.org/docs/List_of_all_log_messages for more info.
   * The example below contains some examples I think some people might find useful.
   * There are no default values.
  */
  notify {
    NEW_GLOBAL_RECORD; // when a new global record number of users has been reached (congratulations btw)
    OPER_FAILED; // someone failed to /OPER
    CONNTHROTTLE_ACTIVATED; // connection throttling has been activated
    HIGH_CONNECTION_RATE; // there is a higher than normal connection rate
    SPAMFILTER_SLOW_FATAL; // the spamfilter is slowing things down to the point of death
    SPAMFILTER_SLOW_WARN; // the spamfilter is being too slow! aaaa
    LINK_DISCONNECTED; // if there is a netsplit (a server disconnected from the network)
  }
}
```
