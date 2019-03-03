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

float tc3() { // check RO segment
  float marks = 0;
  long ptr;
  ptr = (long) expand(10, MAP_RD);
  if (ptr == 0x140000000)
	  marks += 0.5;
  else {
	  write("tc3.1 failed\n", sizeof("tc3.1 failed\n"));
	  return marks;
  }
  ptr = (long) shrink(20, MAP_RD);
  if (ptr == 0)
	  marks += 0.5;
  else {
	  write("tc3.2 failed\n", sizeof("tc3.2 failed\n"));
	  return marks;
  }
  ptr = (long) expand(10, MAP_RD);
  if (ptr == 0x14000A000)
	  marks += 0.5;
  else {
	  write("tc3.3 failed\n", sizeof("tc3.3 failed\n"));
	  print_hex(ptr);
	  return marks;
  }
  ptr = (long) shrink(5, MAP_RD);
  if (ptr == 0x14000F000)
	  marks += 0.0;
  else {
	  write("tc3.4 failed\n", sizeof("tc3.4 failed\n"));
	  return marks;
  }
  ptr = (long) shrink(15, MAP_RD);
  if (ptr == 0x140000000)
	  marks += 0.5;
  else {
	  write("tc3.5 failed\n", sizeof("tc3.5 failed\n"));
	  return marks;
  }
#if 0
  int i;
  for (i = 0; i < 512; ++i) {
  	ptr = (long) expand(512, MAP_RD);
  }
  if (ptr == 0x17FE00000)
	  marks += 0.0;
  else {
	  write("tc3.5 failed\n", sizeof("tc3.5 failed\n"));
	  print_hex(ptr);
	  return marks;
  }
  ptr = (long) expand(1, MAP_RD);
  if (ptr == 0)
	  marks += 0.0;
  else {
	  write("tc3.6 failed\n", sizeof("tc3.6 failed\n"));
	  return marks;
  }
  ptr = (long) shrink(1, MAP_RD);
  if (ptr == 0x17FFFF000)
	  marks += 0.5;
  else {
	  write("tc3.7 failed\n", sizeof("tc3.7 failed\n"));
	  return marks;
  }
#endif
  
  return marks;
}

static int main()
{
  unsigned long i;
  float marks;
#if 0
  unsigned long arr[4096];
  arr[4095] = 45;
#endif
  char marks_str[][18] = {"tc03 marks = 0.0\n", "tc03 marks = 0.5\n",
	  "tc03 marks = 1.0\n", "tc03 marks = 1.5\n", "tc03 marks = 2.0\n"};
#if 1
  i = getpid();
  marks = tc3();
  i = (int) 2 * marks;
  write(marks_str[i], 17);
#endif
  exit(-5);
  return 0;
}
