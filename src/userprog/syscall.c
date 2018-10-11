#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"



static void syscall_handler (struct intr_frame *);
static void verify_pointer(struct intr_frame * esp);
static int write(int fd, void * buf, int size);
static void exit(int status);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
 // printf(">>> system call start!! : %s \n",thread_current()->name);
  struct intr_frame * esp = f-> esp;
  

 // printf(">>> esp pointer value : %p\n",esp);
  verify_pointer(esp);
 // printf(">>>system esp ium  : %d \n", *((int*)esp));  
  int system_call_num = *((int*)esp);  
  int arg[100] = {0}, i = 0;
  switch(system_call_num){
    case  SYS_HALT:
      shutdown_power_off();
      break;
    case SYS_EXIT: 
      arg[0] = *((int*)esp + 1);
      esp -> eax = arg[0];
      exit(arg[0]);      
 
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

static void verify_pointer(struct intr_frame * esp){
  uint32_t *pd = thread_current()->pagedir;
  
  void * result = pagedir_get_page(pd,esp);

  if(result == NULL){
//    printf(">>>wrong Vaddress : %p \n",esp);
    exit(-1); 
  }
  else{
//    printf(">>> CORRECT ADDRESS\n");
  } 
  return;

}


static void exit(int status)
{
  char name[20];
  strlcpy(name,thread_current()->name,20);
 
  int i =0;
  for(i = 0; i < strlen(name) + 1; i++ ){
    if(name[i] == ' '){
      name[i] = '\0';
    }
  }

  printf("%s: exit(%d)\n",name, status);
  list_remove(&thread_current()->child_elem);
  thread_exit();

}

static int write(int fd, void * buf, int size)
{
  if(fd == 1){
    putbuf(buf,size);
    return size;
  }

  return size;
}



