#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
//Added
/* For shutdown poweroff */
#include "devices/shutdown.h"

//Added
typedef int pid_t;
static bool validate_address(void *address);
static void safe_memory_access(void *addr);
static void syscall_handler(struct intr_frame *);

//Added
/* Function prototypes from /usr/sycall.h */
static void halt(void) NO_RETURN;
static void exit(int status) NO_RETURN;
// static pid_t exec (const char *file);
// static int wait (pid_t);
static bool create(const char *file, unsigned initial_size);
// static bool remove (const char *file);
static int open (const char *file);
// static int filesize (int fd);
static int read (int fd, void *buffer, unsigned length);
static int write(int fd, const void *buffer, unsigned length);
// static void seek (int fd, unsigned position);
// static unsigned tell (int fd);
static void close (int fd);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f UNUSED)
{
  //Original
  // printf ("system call!\n");
  // thread_exit ();

  //Added
  safe_memory_access(f->esp);
  switch (*(int *)f->esp)
  {
  case SYS_HALT:
    halt();
    break;
  case SYS_EXIT:
    exit(*((int *)f->esp + 1));
    break;
  case SYS_CREATE:
    create((char *)(*((int *)f->esp + 2)), *((int *)f->esp + 3));
    break;
  case SYS_OPEN:
    open((char *)(*((int *)f->esp + 2)));
    break;
  case SYS_READ:
    read(*((int *)f->esp + 1), (char *)(*((int *)f->esp + 2)), *((size_t *)f->esp + 3));
    break;
  case SYS_WRITE:
    write(*((int *)f->esp + 1), (char *)(*((int *)f->esp + 2)), *((size_t *)f->esp + 3));
    break;
  case SYS_CLOSE:
    close(*((int *)f->esp + 1));
  default:
    printf("error %d", (*(int *)f->esp));
  }
}

static void safe_memory_access(void *addr)
{
  //There are at max 3 arguments that will be in the stack
  int safe_access = validate_address((int*)addr) + validate_address((int*)addr + 1) + 
  validate_address((int*)addr + 2) + validate_address((int*)addr + 3);
  if (safe_access != 4)
    exit(-1);
}

static bool validate_address(void *address)
{
  // check if the pointer is within PHYS_BASE or in the thread's page
  return is_user_vaddr(address) && pagedir_get_page(thread_current()->pagedir, address) != NULL;
}

static void halt()
{
  shutdown_power_off();
}

static void exit(int status)
{
  thread_current()->child_exit_status = status;
  printf("%s: exit(%d)\n", thread_current()->name, status);
  sema_up(&thread_current()->parent->parent_sema);
  thread_exit();
}

static bool create(const char *file, unsigned initial_size UNUSED)
{
  //If no file name
  if (file == NULL)
    exit(-1);
  return 1;  
}

static int open (const char *file){
  if(file==NULL)
    exit(-1);
  return 1;
}

static int read (int fd, void *buffer UNUSED, unsigned length){
  if(length==0)
    exit(-1);
  switch (fd)
  {
  //Uknown descriptor then return error
  default:
    exit(-1);
  }
  //Success
  return 1;
}

static int write(int fd, const void *buffer, unsigned length)
{
  if(length==0 ||buffer==NULL)
    exit(-1);
  switch (fd)
  {
  //Console
  case 1:
    putbuf(buffer, length);
    break;
  //Uknown descriptor then return error
  default:
    exit(-1);
  }
  //Success
  return 1;
}

static void close (int fd){
  return;
}