#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "threads/synch.h"

struct lock filesys_lock;

void syscall_init (void);
void insert_process_list();
void exit(int status);
//void check_correct_pointer(void *);
void insert_process_list();

#endif /* userprog/syscall.h */
