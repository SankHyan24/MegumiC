run:
	cd build; \
	cmake ..; \
	make; \
	./mc -koopa ../test/hello.c -o hello.koopa; \
	cd ../

a:
	cd build;  \
	make;  \
	clear;  \
	./mc; \
	cd ../

c:
	rm hello.o hello.out; \
	cd build;  \
	make;  \
	./mc; \
	cd ../ 
	clang zriscv/target.s -c -o hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32 ; \
	ld.lld hello.o -L${CDE_LIBRARY_PATH}/riscv32 -lsysy -o hello.out; \
	qemu-riscv32-static hello.out

kp:
	koopac hello.koopa | llc --filetype=obj -o hello.o
	clang hello.o -L${CDE_LIBRARY_PATH}/native -lsysy -o hello.out
	./hello.out

risc:
	clang zriscv/target.s -c -o hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
	ld.lld hello.o -L${CDE_LIBRARY_PATH}/riscv32 -lsysy -o hello.out
	qemu-riscv32-static hello.out

clean:
	rm -f hello.o hello.out 