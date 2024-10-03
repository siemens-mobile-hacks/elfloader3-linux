#!/bin/bash
# T10884xxxxxxxxxxxxxxxxxxxxx
gdb-multiarch -ex 'target remote localhost:12309' -ex 'b abort' \
-ex 'c' ./elfloader3
