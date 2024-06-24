## Remove
Based on the module from inspircd, and recommended by Koragg.

### Information
Provides commands `REMOVE` and `FPART`, which can be used to force a user to part the channel.
These commands are available to regular channel operators.

### Syntax:
```
/REMOVE <nick> <channel>

// Example
/REMOVE bob #kitchen
/FPART alice #taylorswift
```

### Configuration
```
remove {
  support-nokicks no; // obey +Q (no-kicks) mode when using this command
  protected-rank "aq"; // will check for and protect EITHER 'a' or 'q'
}
```
