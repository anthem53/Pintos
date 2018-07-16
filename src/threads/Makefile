include ../Makefile.kernel


d:
	pintos -v -k -T 30 --qemu -- -q -mlfqs run mlfqs-recent-1

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
