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
#include "threads/synch.h"

#define FD_NUM 128

struct list process_list;

static void verify_pointer(struct intr_frame * esp);
static void syscall_handler (struct intr_frame *);
static int insert_fd_list(void * file);
static bool is_execute(char * file);

void exit(int status);
static int exec(char * cmd_line);
static int wait (pid_t);
static bool create(const char * file, unsigned initial_size);
static bool remove (const char * file);
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
  list_init(&process_list);
  lock_init(&filesys_lock);
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
      arg[0] = *((int*) esp + i + 1);
      f -> eax = remove((char*)arg[0]);
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

static int insert_fd_list(void * file){
  int i =0;
  struct file_decripter * fd_list = thread_current()->fd_list;

  for(i = 2 ; i < FD_NUM; i++){
    if(fd_list[i].is_open == false){
      fd_list[i].file = file;
      fd_list[i].is_open = true;
      return i;
    }
  }
  return FD_NUM - 1;
}

void check_correct_pointer(void * ptr)
{
  if(is_user_vaddr(ptr) == false){
    exit(-1);
  }
   
  if(pagedir_get_page(thread_current()->pagedir,ptr) == NULL){
    exit(-1);
  }
}

void insert_process_list(){
  list_push_back(&process_list,&thread_current()->process_elem);
  return;
}

static bool is_execute(char * file){

  char my_file[20];
  int i;
  struct list_elem * e = list_begin(&process_list);

  while( e != list_end(&process_list)){
    struct thread * t = list_entry(e, struct thread, process_elem);
     strlcpy(my_file,t->name,20);
     for(i = 0 ; i < 20; i ++ ){
       if(my_file[i] == ' '){
         my_file[i] = '\0';
         break;
       }
       else if( my_file[i] == '\0')
         break;
     } 


    if(strcmp(file,my_file) == 0){
      return true;
 
    }
    else
      e = list_next(e);
  
  }
  
  return false;
}



void exit(int status)
{
  struct thread * cur = thread_current();

  sema_down(&cur->wait_sema);
  cur->is_exit = true;
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

//  printf(">>> status : %d\n",status);
  cur->parent->child_exit_code = status;

 // printf(">>> parent child_exit_code : %d\n",status);
  list_remove(&cur->child_elem);
  
  printf("%s: exit(%d)\n",name, status);
  list_remove(&cur->process_elem);
//  printf(">>> sema address : %p \n",&cur->parent->wait_sema);
  sema_up(&cur->exit_sema);
  thread_exit();

}

static int exec(char * cmd_line)
{
  if(pagedir_get_page(thread_current()->pagedir, cmd_line) == NULL)
    return -1;
  
  char local_cmd_line[150];
  strlcpy(local_cmd_line, cmd_line,150); 
  
  int i = 0;
  for(i = 0 ; i < 150; i++){
    if(local_cmd_line[i] == ' '){
      local_cmd_line[i] = '\0';
      break;
    }
  }
  lock_acquire(&filesys_lock);
  struct file * check_file = filesys_open(local_cmd_line);
  if(check_file == NULL){
    lock_release(&filesys_lock);
    return -1;
  }
  else{
    file_close(check_file);
    lock_release(&filesys_lock);
  }
//  lock_acquire(&filesys_lock);
  tid_t result = process_execute(cmd_line);
//  lock_release(&filesys_lock);
  return result;

}
static int wait (int pid)
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
  lock_acquire(&filesys_lock);
  bool result = filesys_create(file, initial_size);
  lock_release(&filesys_lock);
  return result;  

}

static bool remove (const char * file)
{
  check_correct_pointer(file);
  lock_acquire(&filesys_lock);
  bool result = filesys_remove(file);
  lock_release(&filesys_lock);
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
  bool need_deny = is_execute(file);
//  lock_acquire(&filesys_lock);
  void * file_address = filesys_open(file);

  if(need_deny){
    file_deny_write(file_address);
  }

  
  if(file_address == NULL){
//    lock_release(&filesys_lock);
    return -1;  
  }
  else {
    int result = insert_fd_list(file_address);
//    lock_release(&filesys_lock);
    return result;
  }

}

static int filesize(int fd)
{
  struct file_decripter * fd_list = thread_current()->fd_list; 
  lock_acquire(&filesys_lock);
  int result =file_length(fd_list[fd].file);
  lock_release(&filesys_lock);
  return result;
}


static int read (int fd, void * buffer, off_t size)
{
  if (size == 0)
    return 0;
  if(!(fd == 0 || (fd >= 2 && fd <=FD_NUM)))
    return -1;

  lock_acquire(&filesys_lock);
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
  struct file_decripter * fd_list = thread_current()->fd_list;
  if(fd_list[fd].is_open == false){
    lock_release(&filesys_lock);
    return -1;
  }
  
    int result = file_read(fd_list[fd].file,buffer,size);
  lock_release(&filesys_lock);
  return result;

 
}


static int write(int fd, void * buf, int size)
{
//  printf(">>> arg : fd : %d , size : %d\n",fd, size);
  if(fd == 1){
    putbuf(buf,size);
   // lock_acquire(&filesys_lock);
    return size;
  }
  lock_acquire(&filesys_lock);
  struct file_decripter * fd_list = thread_current()->fd_list;
//  printf(">>> get fd list \n");
  if(fd < 1 || fd >FD_NUM){
//    printf(">>> Not invalid fd\n");
    lock_release(&filesys_lock);
    return 0; 
  }
  else{
    if(fd_list[fd].is_open == false){
//      printf(">>> no open file in fd\n");
      lock_release(&filesys_lock);
      return 0;
    }
  }
 
  check_correct_pointer(buf);

  off_t result = file_write(fd_list[fd].file,buf,size);
//  printf("finish file write \n");
  lock_release(&filesys_lock);
  return result;
}

static void seek(int fd, unsigned pos)
{ 
  if (fd > FD_NUM)
    return;
  
  struct file_decripter * fd_list = thread_current()->fd_list;
  if(fd_list[fd].is_open ==false)
    return ;
  lock_acquire(&filesys_lock); 
  file_seek(fd_list[fd].file, pos);
  lock_release(&filesys_lock);
  return;
    
}

static unsigned tell(int fd)
{
  lock_acquire(&filesys_lock);
  struct file_decripter * fd_list = thread_current()->fd_list;
  unsigned result = file_tell(fd_list[fd].file); 
  lock_release(&filesys_lock);

  return result;
}


static void close(int this_fd)
{
  
  if (this_fd < 2)
    return;
  if (this_fd < 0 ||this_fd > FD_NUM)
    return; 
  lock_acquire(&filesys_lock);
  struct file_decripter * fd_list = thread_current()->fd_list; 
  lock_release(&filesys_lock);

  if(fd_list[this_fd].is_open == true){
    file_close(fd_list[this_fd].file);
    fd_list[this_fd].file = NULL;
    fd_list[this_fd].is_open = false;
  }

}


