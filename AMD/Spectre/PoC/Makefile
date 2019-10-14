all: spectre

spectre: spectre.c
	gcc spectre.c -O0 -o spectre 
	objdump -D spectre > spectre.dis

clean:
	rm -f spectre
