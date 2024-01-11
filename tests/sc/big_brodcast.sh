#!/bin/bash
for i in {0..15} ; do
    ./run_test 1 16 examples_build/brodcast_any_size 100000 $i >/dev/null
done