# Siemens ELF Loader 3.0 emulator for Linux
Alternative implementation of the [@zvova7890](https://github.com/zvova7890) emulator: https://bitbucket.org/vova7890/qemu-siemens-elf/src/master/

This is proof of concept using the original [elfloader 3.0](https://github.com/Alexious-sh/sie-dev/tree/master/elfloader3/src2/loader3) compiled for linux (instead of glibc ld.so in the original emulator).

Currently implemented only a few sets of Siemens Mobile firmware functions. Mostly libc-like functions and partly filesystems.

In this emulator used modified elfloader 3.0 with integrated gdb support.

# Build
```bash
# Required software
sudo apt install crossbuild-essential-armhf libc6-armhf-cross

# Dir with all stuff
mkdir -p ~/dev/sie
cd ~/dev/sie

# SDK with libs
git clone https://github.com/siemens-mobile-hacks/sdk

# Emulator
git clone https://github.com/Azq2/elfloader3-linux
cd elfloader3-linux
cmake .
make
```

# Running elf
```
./elfloader3 ../sdk/libc++/tests/cxx11tests_NSG.elf
```

# Using GDB
```
# One terminal
qemu-arm-static -g 1234 ./elfloader3 ../sdk/libc++/tests/cxx11tests_NSG.elf

# Second terminal
gdb-multiarch -ex 'target remote localhost:1234' ./elfloader3
```
