include ../Makefile.kernel


d:
	pintos -v -k -T 60 --qemu -- -q -mlfqs run mlfqs-load-1

f:
	pintos -v -k -T 60 --qemu -- -q -mlfqs  run mlfqs-nice-2
g:
	pintos -v -k -T 10 --qemu -- -q run priority-change

v:
	vim fixedpoint.h

m:
	vim Makefile
res:
	make clean
	make check
t:
	vim thread.c
s:
	vim synch.c
