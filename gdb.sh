#!/bin/bash
# T10884xxxxxxxxxxxxxxxxxxxxx
gdb -ex 'set debuginfod enabled on' \
-ex 'target remote localhost:1234' \
-ex 'set follow-fork-mode parent' \
-ex 'b abort' \
-ex 'c' ./build/elfloader3
