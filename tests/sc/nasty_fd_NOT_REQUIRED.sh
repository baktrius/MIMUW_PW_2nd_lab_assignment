set -x
for i in {0..128} ; do
    ./run_test_v2 mimpirun2 1 16 examples_build/barrier > /dev/null
done