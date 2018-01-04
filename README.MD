# PS C implementation

A simple C implementation of the `ps x -o pid,comm,rss` command.
Run it as follows: `make && ./ps`.

## Testing

Running the testsuite: `./test.sh`.

**IMPORTANT**


Since the test files aren't created simultaneously and difference occurs very often. In my case Atom used a lot of resources, these changed all the time, which gave me inconsistent test results. The same with the test script itself or larger programs like Firefox and so on. 
