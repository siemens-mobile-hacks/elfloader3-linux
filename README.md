# Siemens ELF Loader 3.0 emulator for Linux
Alternative implementation of the [@zvova7890](https://github.com/zvova7890) emulator: https://bitbucket.org/vova7890/qemu-siemens-elf/src/master/

This is proof of concept using the original [elfloader 3.0](https://github.com/Alexious-sh/sie-dev/tree/master/elfloader3/src2/loader3) compiled for linux (instead of glibc ld.so in the original emulator).

Currently implemented only a few sets of Siemens Mobile firmware functions. Mostly libc-like functions and partly filesystems.

In this emulator used modified elfloader 3.0 with integrated gdb support.

# Prepare for building (Debian / Ubuntu)
1. Create file `/etc/apt/sources.list.d/arm.list` with following content (only for Ubuntu):
```bash
# Change mantic to your distro name
deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports mantic main universe
```

2. Add `[arch=amd64,i386]` for all other sources in the all `/etc/apt/sources.list` and `/etc/apt/sources.list.d/*.list` files (only for Ubuntu).
```bash
# Example

# Before
deb https://ppa.launchpadcontent.net/mozillateam/ppa/ubuntu/ mantic main
deb-src https://ppa.launchpadcontent.net/mozillateam/ppa/ubuntu/ mantic main

# After
deb [arch=amd64,i386] https://ppa.launchpadcontent.net/mozillateam/ppa/ubuntu/ mantic main
deb-src [arch=amd64,i386] https://ppa.launchpadcontent.net/mozillateam/ppa/ubuntu/ lunar main
```

3. Add foreign architecture and update cache:
```bash
sudo apt update
sudo dpkg --add-architecture armhf
```

4. Install packages:
```
sudo apt install crossbuild-essential-armhf libc6-armhf-cross libc6:armhf
```

# Build
```bash
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
