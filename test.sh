#!/bin/bash -e

# rebuild
./build.sh

# run tests
build/src/test_1_reader
build/src/test_2_analyzer
build/src/test_3_eval
build/src/test_4_jit

