include ../Makefile.kernel



create normal:
	pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/create-normal -a create-normal -- -q  -f run create-normal


create long:
	pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/create-long -a create-long -- -q  -f run create-longi


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





