# haveibeenpwned
Check if your password has been leaked (https://haveibeenpwned.com/Passwords)

Contains 613,584,246 passwords.

## Features

1. Converts the colon-separated files into binary compressed columns to save disk space
2. Performs a binary search on-disk - without needing to read the file into memory, meaning memory requirements are very low. This likely means the algorithm will perform a lot worse on a spinning disk, since random access is more expensive.
3. Returns the number of occurrences of the password/hash you searched for.
4. Contains a CPython extension for ease of use.

On my SSD, the lookup + hash typically takes between 100 microseconds and 50 milliseconds. Around 30 random access operations (each reading 20 bytes) is needed to see if a password is among the 613,584,246 SHA1 hashes. (25GB uncompressed)
