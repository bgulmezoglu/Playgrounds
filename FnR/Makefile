all: FnR

FnR: main.c
	gcc main.c -o FnR 
	objdump -D FnR > FnR.dis

clean:
	rm -f FnR
