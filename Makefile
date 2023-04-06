run:
	cd build; \
	cmake ..; \
	make; \
	./mc -koopa ../test/hello.c -o hello.koopa; \
	cd ../




kp:
	koopac hello.koopa | llc --filetype=obj -o hello.o
	clang hello.o -L${CDE_LIBRARY_PATH}/native -lsysy -o hello.out
	./hello.out

risc:
	clang hello.S -c -o hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
	ld.lld hello.o -L${CDE_LIBRARY_PATH}/riscv32 -lsysy -o hello.out
	qemu-riscv32-static hello.out

clean:
	rm -f hello.o hello.out