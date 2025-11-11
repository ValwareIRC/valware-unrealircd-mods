### Date MOTD (UnrealIRCd 6.2.2 or later)
Allows sending specific MOTD lines on specific dates, bundled in with your current MOTD.

Just write out your special MOTD file and write some config for it:

```conf
datemotd {
  item {
    name "christmas";
    file "/home/valerie/christmas.txt";
    date "2025-12-25";
  }
  item {
    name "hate monday";
    file "/home/valerie/hatemonday.txt";
    date "Monday";
  }
}
