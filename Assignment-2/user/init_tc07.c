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

float tc7() { // write on RO, invalid page
#define MARKS_EXPL "tc7: Write on RO and absent page - Give 0.5 marks if this process terminates with an error\n"
#define RO_TEST_FAIL "tc7: Write on RO_Absent_page test failed. Give 0 marks\n"
  float marks = 0;
  int ret;

  long ptr;

  ptr = 0x140000000;
  ret = write(MARKS_EXPL, sizeof(MARKS_EXPL));
  *(long *)ptr = 0xFA12;
  ret = write(RO_TEST_FAIL, sizeof(RO_TEST_FAIL));

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
  char marks_str[][18] = {"tc07 marks = 0.0\n", "tc07 marks = 0.5\n", "tc07 marks = 1.0\n"};

  i = getpid();
  marks = tc7();
  i = (int) 2 * marks;
  write(marks_str[i], 17);
  exit(0);
  return 0;
}
