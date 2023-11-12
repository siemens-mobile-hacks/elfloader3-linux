#!/bin/bash
gdb-multiarch -ex 'target remote localhost:1234' -ex 'b abort' -ex 'c' ./elfloader3
