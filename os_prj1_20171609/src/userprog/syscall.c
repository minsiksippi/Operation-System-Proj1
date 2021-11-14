#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "pagedir.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void check_add(void *address)
{
	struct thread *thrmin = thread_current();
	if(is_user_vaddr(address) && !is_kernel_vaddr(address) && pagedir_get_page(thrmin->pagedir, address))
		;		
	else exit(-1);
}

void user_input(int cnt, void* args[], void* esp)
{
	int operation = 5;
	for(int i=0; i< cnt; i++)
	{
		args[i] = (void*)((unsigned*)esp+i+1);
		check_add(args[i]);
	}
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  void *type = (f->esp);
  void *args[4];

  switch(*(int*)type)
  {
  	case SYS_HALT:
		halt();
		break;
	case SYS_EXIT:
		user_input(1, args, f->esp);
		exit((int)*(int*)args[0]);
		break;
	case SYS_EXEC:
		user_input(1, args, f->esp);
		f->eax = exec((char*)*(unsigned*)args[0]);
		break;
	case SYS_WAIT:
		user_input(1, args, f->esp);
		f->eax = wait((int)*(int*)args[0]);
		break;
	case SYS_WRITE:
		user_input(3, args, f->esp);
		f->eax = write((int)*(int*)args[0], (void*)*(int*)args[1], (size_t)*(int*)args[2]);
		break;
	case SYS_READ:
		user_input(3, args, f->esp);
		f->eax = read((int)*(int*)args[0], (void*)*(int*)args[1], (size_t)*(int*)args[2]);
  		break;
	case SYS_FIBO:
		user_input(1, args, f->esp);
		f->eax = fibonacci((int)*(int*)args[0]);
		break;
	case SYS_MAX:
		user_input(4, args, f->esp);
		f->eax = max_of_four_int((int)*(int*)args[0], (int)*(int*)args[1], (int)*(int*)args[2], (int)*(int*)args[3]);
		break;
 	default:
		break;
  }

}

void halt()
{
	shutdown_power_off();
}

void exit(int status)
{
	struct thread* exitthr = thread_current();
	exitthr->exit_status = status;
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_exit();
}

pid_t exec(const char* cmd_line)
{
	return process_execute(cmd_line);
}

int wait(pid_t pid)
{
	return process_wait(pid);
}

int write(int fd, const void* buffer, unsigned size)
{
	if(fd == 1)
	{
		putbuf((char*)buffer, size);
		return size;
	}
	return -1;
}

int read(int fd, void* buffer, unsigned size)
{
	if(fd == 0)
	{
		for(unsigned int i=0; i<size; i++)
			((char*)buffer)[i]=(char)input_getc();
		return size;
	}
	return -1;
}

int fibonacci(int n)
{
	int fibo1, fibo2, fibo3;
	fibo1 = 1, fibo2 = 1;

	if(n<0) return 0;
	else if(n<3)
		return 1;
	else{
		for(int i=3; i<=n; i++){
			fibo3 = fibo1 + fibo2;
			fibo1 = fibo2;
			fibo2 = fibo3;
		}
		return fibo3;
	}
}

int max_of_four_int(int num1, int num2, int num3, int num4)
{
	int max;
	max = num1;
	if(max< num2) max= num2;
	if(max< num3) max= num3;
	if(max< num4) max= num4;
	return max;
}
