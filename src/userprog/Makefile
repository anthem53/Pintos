include ../Makefile.kernel



sc-bad-sp:
	pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/sc-bad-sp -a sc-bad-sp -- -q  -f run sc-bad-sp


sc-bad-arg:
	pintos -v -k -T 60 --qemu  --filesys-size=2 -p tests/userprog/sc-bad-arg -a sc-bad-arg -- -q  -f run sc-bad-arg

arg_multi test:
	pintos -v -k -T 10 --qemu  --filesys-size=2 -p tests/userprog/args-multiple -a args-multiple -- -q  -f run 'args-multiple some arguments for you!'

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





