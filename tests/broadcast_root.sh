set -ex
for i in {0..15} ; do
    timeout 2s ./mimpirun 16 examples_build/broadcast1 $i
done