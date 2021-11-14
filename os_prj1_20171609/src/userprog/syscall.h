#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

typedef int pid_t;

void syscall_init (void);

void halt(void);
void exit(int status);
pid_t exec(const char *cmd_line);
int wait(pid_t pid);
int write(int fd, const void *buffer, unsigned size);
int read(int fd, void *buffer, unsigned size);

int fibonacci(int n);
int max_of_four_int(int a, int b, int c, int d);

void check_validity_of_address(void *address);
void syscall_argument(int count, void *args[], void* esp);

#endif 
