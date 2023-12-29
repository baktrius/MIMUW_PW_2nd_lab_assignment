set -e
for i in {1..100} ; do
    timeout 1s ./mimpirun 2 examples_build/deadlock1
    timeout 1s ./mimpirun 2 examples_build/deadlock2
    timeout 1s ./mimpirun 2 examples_build/deadlock3
    timeout 1s ./mimpirun 3 examples_build/deadlock4
    timeout 1s ./mimpirun 2 examples_build/deadlock5
    timeout 1s ./mimpirun 2 examples_build/deadlock6
done