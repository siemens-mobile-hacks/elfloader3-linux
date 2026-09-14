# Siemens ELF Loader 3.0 emulator for Linux

This is proof of concept using the original [elfloader 3.0](https://github.com/Alexious-sh/sie-dev/tree/master/elfloader3/src2/loader3) compiled for linux (instead of glibc ld.so in the original emulator).

Currently implemented only a few sets of Siemens Mobile firmware functions. Mostly libc-like functions and partly filesystems.

In this emulator used modified elfloader 3.0 with integrated gdb support.

# Build
```bash
# Required software (Ubuntu)
sudo apt install crossbuild-essential-armhf libc6-armhf-cross

# Dir with all stuff
mkdir -p ~/dev/sie
cd ~/dev/sie

# SDK with libs
git clone https://github.com/siemens-mobile-hacks/sdk

# Emulator
git clone https://github.com/Azq2/elfloader3-linux
cd elfloader3-linux
git submodule init
git submodule update
cmake -B build
cmake --build build -- -j$(nproc)
make
```

# Running elf
```
./build/elfloader3 -V -f ../sdk/libc++/tests/cxx11tests_NSG.elf
```

# Using GDB
```
# One terminal
qemu-arm -g 1234 ./build/elfloader3 -V -f ../sdk/libc++/tests/cxx11tests_NSG.elf

# Second terminal
./gdb.sh
```

# Respect
Original idea by [@zvova7890](https://github.com/zvova7890): https://bitbucket.org/vova7890/qemu-siemens-elf/src/master/

# AI-assisted contributions

We are not against AI. We are against vibe coding, AI slop, and attempts to offload engineering work to a model. This project prioritizes quality, not development speed or results at any cost.

1. **Do not use AI-generated text in human-to-human communication.**

   Write comments, discussions, PR descriptions, and responses to reviewers yourself.

2. **Do not let AI submit PRs or commits.**

   The author must always be a human who has personally reviewed the changes and takes responsibility for them.

4. **Do not submit code primarily designed or written by AI.**

   Architecture, algorithms, code organization, and the final implementation must be decided by a human. AI may only be used as an auxiliary tool.

6. **You must understand all the code you submit.**

   You must be able to explain every change, justify your decisions, and fix any problems yourself. If you do not understand the code, open a feature request instead of a PR.

8. **Code must be simple, clear, and tested.**

   Follow KISS, the project's coding style, and its existing architecture. Do not introduce unnecessary abstractions, dependencies, or untested changes.

AI slop PRs will be closed without review.
