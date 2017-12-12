
#!/bin/bash

STRING="Hello world"
echo $STRING

for i in `seq 1 10`; do
    ./sched_example;
done

