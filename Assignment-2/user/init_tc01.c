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

float tc1() { // write call
  char str[] = "Hello there. Cut 0.5 marks if this sentence is printed\n";
  //char str2[] = "Hello there again.| Cut 0.5 marks if this sentence is printed\n";
  char *str3 = (char *) (0x800000000 + 0x1000000 - 0x2);	// almost end of stack
  int ret, i;
  int len = 13;
  float marks = 0;

  ret = write(str, len);
  if (ret == len)
    marks += 0.5;

#if 0
  for (i = 0; i < sizeof(str2); ++i) {
	  if (str2[i] == '|') {
		  str2[i] = '\0';
		  break;
	  }
  }
  ret = write(str2, sizeof(str2));
  if (1 || ret == i) // always true. cut from total marks as per the string
    marks += 0.5;
#endif
#if 1
  ret = write(str3, 10);
  if (ret == -1) {
    marks += 0.5;
  }
#endif

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
  char marks_str[][18] = {"tc01 marks = 0.0\n", "tc01 marks = 0.5\n", "tc01 marks = 1.0\n", "tc01 marks = 1.5\n"};

  i = getpid();
  marks = tc1();
  i = (int) 2 * marks;
  write(marks_str[i], 17);
  exit(-5);
  return 0;
}
