#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);
static int write(int fd, void * buf, int size);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  void * esp = f-> esp;
  //printf(">>>system esp ium  : %d \n", *((int*)esp));  
  int system_call_num = *((int*)esp);  
  int arg[100] = {0}, i = 0;
  switch(system_call_num){
    case  SYS_HALT:
      printf(">>> halt \n");
      break;
    case SYS_EXIT: 
      printf(">>> exit \n");
      printf(">>> current thread_name : %s\n",thread_current()->name);
      arg[0] = *((int*)esp + 1);
      printf("%s: exit(%d)\n",thread_current()->name, arg[0]);
      thread_exit();
      break;
    case SYS_EXEC : 
      printf(">>> exec \n");
      break;
    case SYS_WAIT :
      printf(">>> wait \n");
      break;
    case SYS_CREATE:
      printf(">>> create \n");
      break;
    case SYS_REMOVE :
      printf(">>> remove \n");
      break;
    case SYS_OPEN :
      printf(">>> open \n");
      break;
    case SYS_FILESIZE :
      printf(">>> filesize \n");
      break;
    case SYS_READ :
      printf(">>> read \n");
      break;
    case SYS_WRITE :
      for(i = 0; i < 3; i ++){
          int * ptr;
          ptr = (int *) esp + 1 + i;
          arg[i]= *ptr;
      }
      write(arg[0],(void*)arg[1],arg[2]);
        
      break;
    case SYS_SEEK :
      printf(">>> seek \n");
      break;
    case SYS_TELL :
      printf(">>> tell \n");
      break;
    case SYS_CLOSE :

      printf(">>> close \n");
  }


}

static int write(int fd, void * buf, int size)
{
  if(fd == 1){
    putbuf(buf,size);
  }

  return size;
}

