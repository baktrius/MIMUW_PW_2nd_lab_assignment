#!/bin/bash
for i in {0..15} ; do
    ./run_test 1 16 examples_build/reduce_any_size 100000 $i >/dev/null
done