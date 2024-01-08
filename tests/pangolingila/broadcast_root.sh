set -ex
for i in {0..15} ; do
    ./run_test 2 16 examples_build/broadcast1 $i
done