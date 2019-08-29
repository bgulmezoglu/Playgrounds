move: move.c
	cc -g move.c -o move
	objdump -D move > move.dis

rename: rename.c
	cc -g rename.c -o rename
	objdump -D rename > rename.dis

add: add.c
	gcc -g add.c -o add
	objdump -D add > add.dis

sum: sum.c
	gcc -g sum.c -o sum
	objdump -D sum > sum.dis

addarray: addarray.c
	gcc -g addarray.c -o addarray
	objdump -D addarray > addarray.dis

sumarray: sumarray.c
	gcc -g sumarray.c -o sumarray
	objdump -D sumarray > sumarray.dis

pointer: pointer.c
	gcc -g pointer.c -o pt
	objdump -D pt > pt.dis

shiftleft: shiftleft.c
	gcc -g shiftleft.c -o shl
	objdump -D shl > shl.dis

meltdown: meltdown.c
	gcc -g meltdown.c -o md
	objdump -D md > md.dis


clear:
	rm -f move
	rm -f move.dis

	rm -f rename
	rm -f rename.dis

	rm -f add
	rm -f add.dis
	
	rm -f sum
	rm -f sum.dis

	rm -f addarray
	rm -f addarray.dis

	rm -f sumarray
	rm -f sumarray.dis

	rm -f pt
	rm -f pt.dis

	rm -f shl
	rm -f shl.dis

	rm -f md
	rm -f md.dis