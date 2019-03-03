#include<init.h>
#include<memory.h>
static void exit(int);
static int main(void);


void init_start()
{
  int retval = main();
  exit(0);
}



/*Invoke system call with no additional arguments*/
static long _syscall0(int syscall_num)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

/*Invoke system call with one argument*/

static long _syscall1(int syscall_num, int exit_code)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}
/*Invoke system call with two arguments*/

static long _syscall2(int syscall_num, u64 arg1, u64 arg2)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

static long _syscall3(int syscall_num, u64 ticks)
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

static long _syscall4(int syscall_num, u64 signo, void (*sigHandler)(int) )
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

static long _syscall_for_clone(int syscall_num, void *func , void *rsp )
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

static u64* _syscall_for_expand(int syscall_num, u32 size, int flags )
{
  asm volatile ( "int $0x80;"
                 "leaveq;"
                 "retq;"
                :::"memory");
  return 0;   /*gcc shutup!*/
}

static u64 expand(u32 size, int flags)
{
  return (_syscall_for_expand(SYSCALL_EXPAND, size, flags)); 
}


static void exit(int code)
{
  _syscall1(SYSCALL_EXIT, code);
}

static long getpid()
{
  return(_syscall0(SYSCALL_GETPID));
}

static long write(char *ptr, int size)
{
   return(_syscall2(SYSCALL_WRITE, (u64)ptr, size));
}

static long alarm(u64 ticks)
{
   return(_syscall1(SYSCALL_ALARM, ticks));
}

static long sleep(u64 ticks)
{
   return(_syscall1(SYSCALL_SLEEP, ticks));
}

static long signalRegister(u64 signo, void (*sigHandler)(int)  )
{
   return(_syscall4(SYSCALL_SIGNAL, signo, sigHandler));
}

static long clone(void *func, void *rsp  )
{
   return(_syscall_for_clone(SYSCALL_CLONE, func, rsp));
}

static void alarmHandler(int signo){
  int x=write("Alarm has rung\n",16);
  return;
}

static void SegHandler(int signo){
  int x=write("Segfault has rung\n",16);
  return;
}

static void child(){
    write("I am child\n",11);
    exit(1);
    return; 
}

static int main()
{

  //int i=0;
  //int j = 5/i;
  //signalRegister(SIGSEGV, &SegHandler);
  //char *str="Starting Program\n";
  //int x=write(str,18);  
  // unsigned long i, j;
  // unsigned long buff[4096];
  // i = getpid();
   //u64* outOfRange=0x0;    /*Segmentation Fault*/

  // for(i=0; i<4096; ++i){
  //     j = buff[i];
  // }
  //alarm(5);
  //int i=0x100034;
  //int j = i / (i-0x100030);
   //j=*outOfRange;
  // exit(-5);

  signalRegister(SIGALRM, &alarmHandler);
  //alarm(2);
  //sleep(2); 
  //alarm(3);
  /*j=j+1;
  alarm(2);
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  j=j+1;
  //alarm(3);
  j=j+1;
  j=j+1;
  j=j+1;*/
  sleep(10);
  //int a[2];
  //int z=a[3];

  // exit(2);
    char* child_stack_ptr = (char*) expand(50,MAP_WR);
    child_stack_ptr[0] = 0;
    //child();

  // exit(66);
  for(int i=0;i<512;i++) child_stack_ptr[i]=0;
  //sleep(3);
  u64 c1 = clone(&child, child_stack_ptr);
  //sleep(8);	  
  //sleep(1);
  //u64 c2 = clone(&child, child_stack_ptr);
  //sleep(3);
  //write("Handled PF\n",11);  
  //while(1);
	
  //int i=0x100034;
  //int j = i / (i-0x100034);

  //write("Closing Program\n",18);  
  //exit(1);
  return 0;
}
