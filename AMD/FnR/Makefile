all: fnr

fnr: main.c
	gcc main.c -O0 -o fnr
	objdump -D fnr > fnr.dis

clean:
	rm -f fnr
	rm -f fnr.dis
