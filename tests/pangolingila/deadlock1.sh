set -e
if [ -z ${VALGRIND+x} ]; then
    inter=100
else
    inter=10
fi
for i in {1..$inter} ; do
    ./run_test 1 2 examples_build/deadlock1
    ./run_test 1 2 examples_build/deadlock2
    ./run_test 1 2 examples_build/deadlock3
    ./run_test 1 3 examples_build/deadlock4
    ./run_test 1 2 examples_build/deadlock5
    ./run_test 5 2 examples_build/deadlock6
done