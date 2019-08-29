all: meltdown

meltdown: main.c
	gcc main.c -O0 -o meltdown 
	objdump -D meltdown > meltdown.dis

clean:
	rm -f meltdown
