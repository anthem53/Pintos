#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

void kill_process(int status);
void check_correct_pointer(void *);


#endif /* userprog/syscall.h */
