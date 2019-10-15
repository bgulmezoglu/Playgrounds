all: 	spectre1 spectre2

spectre1: spectre1.c
	gcc spectre1.c -O0 -o spectre1 
	objdump -D spectre1 > spectre1.dis

spectre2: spectre2.c
	gcc spectre2.c -O0 -o spectre2 
	objdump -D spectre2 > spectre2.dis
clean:
	rm -f spectre1
	rm -f spectre1.dis
	rm -f spectre2
	rm -f spectre2.dis
