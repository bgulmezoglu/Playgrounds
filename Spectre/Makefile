all: spectre

spectre: spectre.c utils.c utils.h
	gcc spectre.c utils.c utils.h -O0 -o spectre 
	objdump -D spectre > spectre.dis

clean:
	rm -f spectre
