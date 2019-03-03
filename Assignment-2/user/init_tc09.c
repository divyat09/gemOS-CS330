#include<init.h>
static void exit(int);
static int main(void);


void init_start()
{
  int retval = main();
  exit(0);
}

/*Invoke system call with no additional arguments*/
static int _syscall0(int syscall_num)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

/*Invoke system call with one argument*/

static int _syscall1(int syscall_num, int exit_code)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

/*Invoke system call with two arguments*/

static long _syscall2(int syscall_num, unsigned long arg1, unsigned long arg2)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}


static void exit(int code)
{
  _syscall1(SYSCALL_EXIT, code); 
}

static int getpid()
{
  return(_syscall0(SYSCALL_GETPID));
} 

#define MAP_RD  0x0
#define MAP_WR  0x1

static int write(const char *str, unsigned long len) {
  return _syscall2(SYSCALL_WRITE, (unsigned long)str, len);
}

static void *expand(unsigned size, int flags) {
	return (void *)_syscall2(SYSCALL_EXPAND, size, flags);
}

static void *shrink(unsigned size, int flags) {
	return (void *)_syscall2(SYSCALL_SHRINK, size, flags);
}

float tc9() { // write on RO, invalid page
#define MARKS_EXPL "tc9: Write on unknown segment - Give 0.5 marks if this process terminates with an error\n"
#define RO_TEST_FAIL "tc9: Write on unknown segment test failed. Give 0 marks\n"
  float marks = 0;
  int ret;
  unsigned long rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15;

  long ptr;
  long ptr_val;
  ptr = (long) expand(10, MAP_RD);
  if (ptr == 0x140000000)
	  marks += 0.0;
  else {
	  write("tc9.1 init failed\n", sizeof("tc9.1 init failed\n"));
	  return marks;
  }
        asm volatile(
                        "mov $1, %%rax;"
                        "mov $2, %%rbx;"
                        "mov $3, %%rcx;"
                        "mov $4, %%rdx;"
                        "mov $5, %%r8;"
                        "mov $6, %%r9;"
                        "mov $7, %%r10;"
                        "mov $8, %%r11;"
                        "mov $9, %%r12;"
                        "mov $10, %%r13;"
                        "mov $11, %%r14;"
                        "mov $12, %%r15;"
                        :::"memory"
                    );
  ptr_val = *(long *)ptr;
        /*asm volatile(
                        "mov %rax, -0x38(%rbp);"
                        "mov %rbx, -0x40(%rbp);"
                        "mov %rcx, -0x48(%rbp);"
                        "mov %rdx, -0x50(%rbp);"
                        "mov %r8, -0x58(%rbp);"
                        "mov %r9, -0x60(%rbp);"
                        "mov %r10, -0x68(%rbp);"
                        "mov %r11, -0x70(%rbp);"
                        "mov %r12, -0x78(%rbp);"
                        "mov %r13, -0x80(%rbp);"
                        "mov %r14, -0x88(%rbp);"
			"mov %r15, -0x90(%rbp);"
                    );*/
        asm volatile(
                        "mov %%rax, %0;"
                        "mov %%rbx, %1;"
                        "mov %%rcx, %2;"
                        "mov %%rdx, %3;"
                        "mov %%r8, %4;"
                        "mov %%r9, %5;"
                        "mov %%r10, %6;"
                        "mov %%r11, %7;"
                        "mov %%r12, %8;"
                        "mov %%r13, %9;"
                        "mov %%r14, %10;"
                        "mov %%r15, %11;"
                        : "=m" (rax), "=m" (rbx), "=m" (rcx), "=m" (rdx),
			  "=m" (r8), "=m" (r9), "=m" (r10), "=m" (r11),
			  "=m" (r12), "=m" (r13), "=m" (r14), "=m" (r15)
			:
			:"memory"
                    );
  rax = rax + 1;
  if (rax < 0 || rbx != 2 || rcx != 3 || rdx != 4
		  || r8 != 5 || r9 != 6 || r10 != 7 || r11 != 8
		  || r12 != 9 || r13 != 10 || r14 != 11 || r15 != 12)
	  write("tc9.1 failed\n", sizeof("tc9.1 failed\n"));
  else
	  marks += 1.0;
  return marks;
}

static int main()
{
  unsigned long i;
  float marks;
#if 0
  unsigned long *ptr = (unsigned long *)0x100032;
  i = *ptr;
#endif
  char marks_str[][18] = {"tc09 marks = 0.0\n", "tc09 marks = 0.5\n", "tc09 marks = 1.0\n"};

  i = getpid();
  marks = tc9();
  i = (int) 2 * marks;
  write(marks_str[i], 17);
  exit(0);
  return 0;
}
