
build_bin:
	clang test/a.s -c -o hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32 ; \
	ld.lld hello.o -L${CDE_LIBRARY_PATH}/riscv32 -lsysy -o hello.out; \

test1: 
	make clean; \
	cd build;  \
	make;  \
	./mc ../test/task1.c; \
	cd ../
	make build_bin
	./tester/quicksort-linux-amd64 ./run.sh
	make clean

test2:
	make clean; \
	cd build;  \
	make;  \
	./mc ../test/task2.c; \
	cd ../
	make build_bin
	./tester/matrix-linux-amd64 ./run.sh
	make clean

test3:
	make clean; \
	cd build;  \
	make;  \
	./mc ../test/task3.c; \
	cd ../
	make build_bin
	./tester/advisor-linux-amd64 ./run.sh
	make clean
	


koopa:
	koopac hello.koopa | llc --filetype=obj -o hello.o
	clang hello.o -L${CDE_LIBRARY_PATH}/native -lsysy -o hello.out
	./hello.out

risc:
	clang zriscv/target.s -c -o hello.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
	ld.lld hello.o -L${CDE_LIBRARY_PATH}/riscv32 -lsysy -o hello.out
	qemu-riscv32-static hello.out

clean:
	rm -f *.o *.out 
	rm -rf *.out 