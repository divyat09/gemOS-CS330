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

static int _syscall2(int syscall_num, unsigned long arg1, unsigned long arg2)
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

static int write(const char *str, unsigned long len) {
  return _syscall2(SYSCALL_WRITE, (unsigned long)str, len);
}

float tc10() { // div/0
#define MARKS_EXPL "tc10: Divide by 0 - Give 1.0 marks if this process terminates with an error\n"
#define DIV_BY_0_FAIL "tc10: Divide/0 failed. Give 0 marks\n"
  float marks = 0;
  int i, ret;

  ret = write(MARKS_EXPL, sizeof(MARKS_EXPL));
  i = ret - ret;
  i = ret / i;
  ret = write(DIV_BY_0_FAIL, sizeof(DIV_BY_0_FAIL));

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
  char marks_str[][18] = {"tc10 marks = 0.0\n", "tc10 marks = 0.5\n", "tc10 marks = 1.0\n"};

  i = getpid();
  marks = tc10();
  i = (int) 2 * marks;
  write(marks_str[i], 4);
  exit(0);
  return 0;
}
