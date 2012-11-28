#!/bin/bash -e

#wc -l `find src -name *.cpp` `find include -name *.hpp` `find include -name *.tpp`

cloc src/ include/

