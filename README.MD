# PS

A simple C implementation of the `ps x -o pid,comm,rss` command.
Run it as follows: `make && ./ps`.

## Testing

Running the testsuite: `./test.sh`.

**IMPORTANT**
Since the test file aren't created simultaneously sometimes there might be a
difference. In my case Atom used a lot of resources which changed all the time
which gave me inconsistent test results. The same with the test script itself.
