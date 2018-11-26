# euler_path
Tool to find eulerian path using igraph library's graph

### Prerequisites

You need to install gengetopt, automake, libxml2-dev and igraph library

### Installing

Execute makeFile

```
make
```

Run the script

```
./main.exe -f test.txt -t e -o output.txt
```

--help or -h are the options to get more infos about what options can
  be passed.

### Output 

The output of the file is the sequence of nodes which has to be
visited in oreder to create an eulerian path or cycle.
