# Email module for UnrealIRCd
Send emails about the logs that you need to know about, even when you're away from IRC.

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

  // the event ID of the log that you wish to be notified about
  // below are some examples of what might be useful.
  // there are no default values.
  notify {
    CONNTHROTLE_ACTIVATED;
    HIGH_CONNECTION_RATE;
    SPAMFILTER_SLOW_FATAL;
    SPAMFILTER_SLOW_WARN;
  }
}
```
