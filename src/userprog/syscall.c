#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/off_t.h"


typedef int pid_t;

struct file_decripter{
  void * file;
  char name[15];
  bool is_open;
}; 


struct file_decripter fd_list[100];

static void syscall_handler (struct intr_frame *);
static void verify_pointer(struct intr_frame * esp);
static int insert_fd_list(void * file, char * name);
static void check_correct_pointer(void *);


static void exit(int status);
static pid_t exec(char * cmd_line);
static int wait (pid_t);
static bool create(const char * file, unsigned initial_size);
static int open (char * file);
static int filesize(int fd);
static int read (int fd, void * buffer, off_t size);
static void close(int fd);
static int write(int fd, void * buf, int size);
static void seek(int fd, unsigned pos);
static unsigned tell (int fd);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  memset(fd_list,0,100);

}

static void
syscall_handler (struct intr_frame *f) 
{
  struct intr_frame * esp = f-> esp;
  
  verify_pointer(esp);
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
      arg[0] = *((int*)esp + 1);
      f -> eax = exec((char *) arg[0]);    

      break;
    case SYS_WAIT :
      arg[0] = *((int *) esp + 1);

      f -> eax = wait(arg[0]);
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
      arg[0] = *((int*)esp + 1 );
      f-> eax = filesize(arg[0]);

      break;
    case SYS_READ :
      for(i = 0; i < 3; i++){
        arg[i] = *((int *)esp + 1 + i);
      }
      f -> eax = read(arg[0],(void *)arg[1],(off_t)arg[2]);
      break;
    case SYS_WRITE :
      for(i = 0; i < 3; i ++){
          int * ptr;
          ptr = (int *) esp + 1 + i;
          arg[i]= *ptr;
      }
      f -> eax = write(arg[0],(void*)arg[1],arg[2]);
        
      break;
    case SYS_SEEK :
      for(i = 0 ; i < 2 ; i++){
        arg[i] = *((int*)esp +1 + i);
      }
      seek(arg[0],(unsigned)arg[1]);
 
      break;
    case SYS_TELL :
      arg[0] = *((int*) esp + i + 1);
      f -> eax = tell(arg[0]);

      //printf(">>> tell \n");
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
    exit(-1); 
  }
  return;

}

static int insert_fd_list(void * file, char * name){
  int i =0;

  for(i = 2 ; i < 100; i++){
    if(fd_list[i].is_open == false){
      fd_list[i].file = file;
      strlcpy(fd_list[i].name, name , 14);
      fd_list[i].is_open = true;
      return i;
      break;
    }
  }

}

static void check_correct_pointer(void * ptr)
{
 // printf(">>> buffer ptr : %p\n",ptr);
  if(is_user_vaddr(ptr) == false){
   // printf(">>> Over PHYS_BASE : %p\n",ptr);
    exit(-1);
  }
   

  if(pagedir_get_page(thread_current()->pagedir,ptr) == NULL){
   // printf(">>> Fail to get page of ptr(%p)\n",ptr);
    exit(-1);
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
  thread_current()->parent->child_exit_code = status;
  printf("%s: exit(%d)\n",name, status);
  list_remove(&thread_current()->child_elem);
  thread_exit();

}

static pid_t exec(char * cmd_line)
{
  if(pagedir_get_page(thread_current()->pagedir, cmd_line) == NULL)
    return -1;

  char local_cmd_line[150];
  strlcpy(local_cmd_line, cmd_line,150); 
  //printf(">>> local cmd line file name : %s\n",local_cmd_line);

  int i = 0;
  for(i = 0 ; i < 150; i++){
    if(local_cmd_line[i] == ' '){
      local_cmd_line[i] = '\0';
      break;
    }
  }

  //printf(">>> local cmd line file name : %s\n",local_cmd_line);

  struct file * check_file = filesys_open(local_cmd_line);
  if(check_file == NULL){
  //printf(">>> file is not existed : %s\n",local_cmd_line);
    return -1;
  }
  tid_t result = process_execute(cmd_line);
  return result;

}
static int wait (pid_t pid)
{
  int result = process_wait(pid);
 
  return result;
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

static int filesize(int fd)
{
  return file_length(fd_list[fd].file);
}


static int read (int fd, void * buffer, off_t size)
{
//  printf(">>> buffer address : %p\n",buffer);
  if (size == 0)
    return 0;
  if(!(fd == 0 || (fd >= 2 && fd <=100)))
    return -1;

  /* stdout*/
  if(fd == 0){
    int i;
    for(i = 0; i < size; i ++){
      char input = input_getc();
      if(input == 10)
        break;
      *((char*)buffer + i) = input; 
    }
    return i;
  }
  check_correct_pointer(buffer);
  if(fd_list[fd].is_open == false)
    return -1;
  
  void * file = fd_list[fd].file;

  int result = file_read(fd_list[fd].file,buffer,size);
  return result;

  
}


static int write(int fd, void * buf, int size)
{
  if(fd == 1){
    putbuf(buf,size);
    return size;
  }

  if(fd < 1 || fd >100){
    return 0; 
  }
  else{
    if(fd_list[fd].is_open == false)
      return 0;
  }
 
  check_correct_pointer(buf);

  off_t result = file_write(fd_list[fd].file,buf,size);

  return result;
}

static void seek(int fd, unsigned pos)
{
  if (fd > 100)
    return;

  if(fd_list[fd].is_open ==false)
    return ;

  file_seek(fd_list[fd].file, pos);
  return;
    
}

static unsigned tell(int fd)
{
  return file_tell(fd_list[fd].file); 
}


static void close(int this_fd)
{
  if (this_fd < 2)
    return;
  if (this_fd < 0 ||this_fd > 100)
    return;  

  if(fd_list[this_fd].is_open == true){
    file_close(fd_list[this_fd].file);
    fd_list[this_fd].file = NULL;
    fd_list[this_fd].name[0] = '\0';
    fd_list[this_fd].is_open = false;
  }

}



