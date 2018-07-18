include ../Makefile.kernel


d:
	pintos -v -k -T 100 --qemu -- -q -mlfqs run mlfqs-load-avg

f:
	pintos -v -k -T 60 --qemu -- -q -mlfqs  run mlfqs-nice-2

v:
	vim fixedpoint.h

m:
	vim Makefile
res:
	make clean
	make check
t:
	vim thread.c
t2:
	vim timer.c
s:
	vim synch.c
