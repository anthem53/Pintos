include ../Makefile.kernel



exec missing:
	pintos -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/exec-missing -a exec-missing -- -q  -f run exec-missing 




read boundary:
	pinntos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/read-boundary -a read-boundary -p ../../tests/userprog/sample.txt -a sample.txt -- -q  -f run read-boundary


create empty:
	pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/create-empty -a create-empty -- -q  -f run create-empty 


s:
	vim syscall.c

i:
	vim interrupt.h


r:
	vim process.c
m:
	vim Makefile

res:
	make clean
	make check





