include ../Makefile.kernel



arg_none test:
	pintos -v -k -T 1 --qemu  --filesys-size=2 -p tests/userprog/args-none -a args-none -- -q  -f run args-none

arg_single test:
	pintos -v -k -T 1 --qemu  --filesys-size=2 -p tests/userprog/args-single -a args-single -- -q  -f run 'args-single onearg'

arg_multi test:
	pintos -v -k -T 10 --qemu  --filesys-size=2 -p tests/userprog/args-multiple -a args-multiple -- -q  -f run 'args-multiple some arguments for you!'

s:
	vim syscall.c

i:
	vim interrupt.h
t:
	cat test


r:
	vim process.c
m:
	vim Makefile

res:
	make clean
	make check





