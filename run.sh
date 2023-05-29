#! /bin/bash
# rm hello.o hello.out; \
# cd build;  \
# make;  \
# ./mc 0 ../test/task2.c; >>/dev/null \
# cd ../; \
# clang zriscv/target.s -c -o hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32 ; \
# ld.lld hello.o -L${CDE_LIBRARY_PATH}/riscv32 -lsysy -o hello.out; \
qemu-riscv32-static ./hello.out
