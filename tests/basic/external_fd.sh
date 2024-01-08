#!/bin/bash
set -e
path=`mktemp`
exec 7<> "$path"  #open fd 7 for our children
./run_test 0.4 2 examples_build/external_fd
echo -e "Process 1 received number from process 0" | diff "$path" -
# don't close the file, it will be closed for us anyway and script is simpler
# exec 7>&- #close fd 7.
