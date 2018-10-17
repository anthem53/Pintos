#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

struct file_decripter{
  void * file;
  char name[15];
  bool is_open;
}; 


struct file_decripter fd[100];

static void syscall_handler (struct intr_frame *);
static void verify_pointer(struct intr_frame * esp);
static int insert_fd_list(void * file, char * name);

static void exit(int status);
static bool create(const char * file, unsigned initial_size);
static int open (char * file);
static void close(int fd);
static int write(int fd, void * buf, int size);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  memset(fd,0,100);

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
      f -> eax = arg[0];
      exit(arg[0]);      
 
      break;
    case SYS_EXEC : 
      printf(">>> exec \n");
      break;
    case SYS_WAIT :
      printf(">>> wait \n");
      break;
    case SYS_CREATE:
      for(i = 0; i < 2; i++)
        arg[i] = *((int*)esp + 1 + i);

      bool result = create((char*)arg[0],(unsigned)arg[1]);
      f -> eax = (uint32_t)result;
      break;
    case SYS_REMOVE :
      printf(">>> remove \n");
      break;
    case SYS_OPEN :
      arg[0] = *((int *)esp + 1);
      int result_fd = open((char*)arg[0]);
      f->eax = result_fd;
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
      arg[0] = *((int*)esp + 1);
      close(arg[0]);
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

static int insert_fd_list(void * file, char * name){
  int i =0;

  for(i = 2 ; i < 100; i++){
    if(fd[i].is_open == false){
      fd[i].file = file;
      strlcpy(fd[i].name, name , 14);
      fd[i].is_open = true;
      return i;
      break;
    }
  }

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
  
  if (status < 0)
    status = -1;
  printf("%s: exit(%d)\n",name, status);
  list_remove(&thread_current()->child_elem);
  thread_exit();

}

static bool create(const char * file, unsigned initial_size){

  uint32_t * pd = thread_current()->pagedir;
  /* check correct file*/ 

  void * page = pagedir_get_page(pd,(void*)file);
  if(page == NULL)
    exit(-1) ;
  

   if(file == NULL) 
    exit(-1);

  /* check string correct*/
  int len = strlen(file);
  if(len == 0 || len > 14){
    return false;
  }


  /* already existed*/
  bool result = filesys_create(file, initial_size);
  return result;  

}

static int open(char * file)
{
  if (file == NULL)
    exit(-1);
  if(pagedir_get_page(thread_current()->pagedir,(void*)file) == NULL)
    exit(-1);

  int len = strlen(file);
  if(len == 0)
    return -1;
  void * file_address = filesys_open(file);
  
  if(file_address == NULL)
    return -1;  
  else {
    int result = insert_fd_list(file_address,file);
    return result;
  }

}

static void close(int this_fd)
{
  if (this_fd < 2)
    return;
  if (this_fd < 0 ||this_fd > 100)
    return;  

  if(fd[this_fd].is_open == true){
    file_close(fd[this_fd].file);
    fd[this_fd].file = NULL;
    fd[this_fd].name[0] = '\0';
    fd[this_fd].is_open = false;
  }

}

static int write(int fd, void * buf, int size)
{
  if(fd == 1){
    putbuf(buf,size);
    return size;
  }

  return size;
}



