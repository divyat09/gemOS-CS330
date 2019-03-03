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

static void print_hex(long num) {
	if (num == 0)
		write("0", 1);
	else while (num) {
		int x = num % 16;
		char c = x < 10 ? '0' + x : 'A' - 10 + x;
		write(&c, 1);
		num /= 16;
	}
	write("\n", 1);
}

float tc5() { // check page fault
  float marks = 0;
  long ptr;
  long *ptr_p;
  int i;
  long values[] = {0xBAAAFFFFABCD0101, 0xBAAAFFFFABCD0101, 0xDDDDAAAABBBB3333,
	  0x45451234AAAABBBB, 0xAAAAAAAAAAAAAAAA, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000};
  int len = sizeof(values) / sizeof(values[0]);
  ptr_p = expand(1, MAP_WR);
  ptr = (long) ptr_p;
  if (ptr == 0x180001000)
	  marks += 0.0;
  else {
	  write("tc5.1 init failed\n", sizeof("tc5.1 init failed\n"));
	  return marks;
  }
  for (i = 0; i < len; ++i) {
	  *(ptr_p + i) = values[i];
  }
  for (i = 0; i < len; ++i) {
	  if (*(ptr_p + i) != values[i]) {
		  write("tc5.1 check failed\n", sizeof("tc5.1 check failed\n"));
	          return marks;
	  }
  }
  marks += 0.5;

  ptr_p = expand(1, MAP_WR);
  ptr = (long) ptr_p;
  if (ptr == 0x180002000)
	  marks += 0.0;
  else {
	  write("tc5.2 init failed\n", sizeof("tc5.2 init failed\n"));
	  return marks;
  }
  for (i = 0; i < len; ++i) {
	  *(ptr_p + i) = values[len - i - 1];
  }
  for (i = 0; i < len; ++i) {
	  if (*(ptr_p + i) != values[len - i - 1]) {
		  write("tc5.2 check failed\n", sizeof("tc5.2 check failed\n"));
	          return marks;
	  }
  }
  marks += 0.5;
#define SOME_MARKS_EXPL "Give +0.5 marks if there is a seg fault now AND the process is killed\n"
#define SOME_MARKS_EXPL_2 "Process is not killed. No extra 0.5 marks\n"

  ptr = (long) shrink(1, MAP_WR);
  if (ptr == 0x180002000)
  	marks += 0.5;
  else {
  	write("tc5.3 init failed\n", sizeof("tc5.3 init failed\n"));
	return marks;
  }

  char marks_str[][18] = {"tc05 marks = 0.0\n", "tc05 marks = 0.5\n",
	  "tc05 marks = 1.0\n", "tc05 marks = 1.5\n", "tc05 marks = 2.0\n"};

  i = (int) 2 * marks;
  write(marks_str[i], 17);

  write(SOME_MARKS_EXPL, sizeof(SOME_MARKS_EXPL));
  *(ptr_p + 1) = 0xFA12;
  write(SOME_MARKS_EXPL_2, sizeof(SOME_MARKS_EXPL_2));
  return -1;
}

static int main()
{
  unsigned long i;
  float marks;
#if 0
  unsigned long *ptr = (unsigned long *)0x100032;
  i = *ptr;
#endif
  char marks_str[][18] = {"tc05 marks = 0.0\n", "tc05 marks = 0.5\n",
	  "tc05 marks = 1.0\n", "tc05 marks = 1.5\n", "tc05 marks = 2.0\n"};

  i = getpid();
  marks = tc5();

  if (marks >= 0) {
	  i = (int) 2 * marks;
	  write(marks_str[i], 17);
  }
  exit(-5);
  return 0;
}
