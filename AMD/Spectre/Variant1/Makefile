all: spectre1

spectre1: spectre.c
	gcc spectre.c -O0 -o spectre1
	objdump -D spectre1 > spectre1.dis

clean:
	rm -f spectre1
	rm -f spectre1.dis
