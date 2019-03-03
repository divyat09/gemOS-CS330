#include<context.h>
#include<memory.h>
#include<schedule.h>
static u64 numticks;


static void schedule_context(struct exec_context *next)
{
  /*Your code goes in here. get_current_ctx() still returns the old context*/
 struct exec_context *current = get_current_ctx();
 printf("schedluing: old pid = %d  new pid  = %d\n", current->pid, next->pid); /*XXX: Don't remove*/
/*These two lines must be executed*/
 set_tss_stack_ptr(next);
 set_current_ctx(next);
 return;
}


static void clean_curr_process(){

    u64* RBP;
    asm volatile ("mov %%rbp, %0;" : "=r"( RBP )::"memory" );

    os_pfn_free(OS_PT_REG, *(RBP+2));    

    asm volatile ( "mov %%rbp, %%rsp;"
          "pop %%rdi;"
          "pop %%rdi;"
          "pop %%rdi;"
          "pop %%rsi;"
          "pop %%rdx;"
          "pop %%rcx;"
          "pop %%rbx;"
          "pop %%rax;"
          "pop %%r15;"
          "pop %%r14;"
          "pop %%r13;"
          "pop %%r12;"
          "pop %%r11;"
          "pop %%r10;"
          "pop %%r9;"
          "pop %%r8;"
          "pop %%rbp;"
          "iretq;"
          : 
          :
          :"memory"
      );

}

static void save_exit_context( struct exec_context* curr, struct exec_context* next  ){    
    
    u64* RBP = osmap( next->os_stack_pfn );
    u64* RSP = RBP - 20;

    *(RSP+0)= curr->os_stack_pfn;

    *(RSP+1)= next->regs.rdi;
    *(RSP+2)= next->regs.rsi;
    *(RSP+3)= next->regs.rdx;
    *(RSP+4)= next->regs.rcx;
    *(RSP+5)= next->regs.rbx;
    *(RSP+6)= next->regs.rax;

    *(RSP+7)= next->regs.r15;
    *(RSP+8)= next->regs.r14;
    *(RSP+9)= next->regs.r13;
    *(RSP+10)= next->regs.r12;
    *(RSP+11)= next->regs.r11;
    *(RSP+12)= next->regs.r10;
    *(RSP+13)= next->regs.r9;
    *(RSP+14)= next->regs.r8;

    *(RSP+15)= next->regs.rbp;
    *(RSP+16)= next->regs.entry_rip;
    *(RSP+17)= next->regs.entry_cs;
    *(RSP+18)= next->regs.entry_rflags;
    *(RSP+19)= next->regs.entry_rsp;
    *(RSP+20)= next->regs.entry_ss;

    asm volatile ("mov %0, %%rsp;" :: "r" (RSP):"memory" );    

    clean_curr_process();
    return;

}

static void save_timer_context( struct exec_context* curr, struct exec_context* next, u64* RBP, u64* RSP  ) {
  /*Your code goes in here*/ 

    // Storing
    curr->regs.rdi = *(RSP + 0);
    curr->regs.rsi = *(RSP + 1);
    curr->regs.rdx = *(RSP + 2);
    curr->regs.rcx = *(RSP + 3);
    curr->regs.rbx = *(RSP + 4);
    curr->regs.rax = *(RSP + 5);
    curr->regs.r15 = *(RSP + 6);
    curr->regs.r14 = *(RSP + 7);
    curr->regs.r13 = *(RSP + 8);
    curr->regs.r12 = *(RSP + 9);
    curr->regs.r11 = *(RSP + 10);
    curr->regs.r10 = *(RSP + 11);
    curr->regs.r9 = *(RSP + 12);
    curr->regs.r8 = *(RSP + 13);

    curr->regs.rbp = *(RBP + 0);
    curr->regs.entry_rip = *(RBP + 1);
    curr->regs.entry_cs = *(RBP + 2);
    curr->regs.entry_rflags = *(RBP + 3);
    curr->regs.entry_rsp = *(RBP + 4);
    curr->regs.entry_ss = *(RBP + 5);


    // u64 *RBP;
    // asm volatile ("movq %%rbp, %0" : "=r"(RBP));

    *(RBP)=next->regs.rbp;
    *(RBP+1)=next->regs.entry_rip;
    *(RBP+2)=next->regs.entry_cs;
    *(RBP+3)=next->regs.entry_rflags;
    *(RBP+4)=next->regs.entry_rsp;
    *(RBP+5)=next->regs.entry_ss;

    
    *(RSP+0)= next->regs.rdi;
    *(RSP+1)= next->regs.rsi;
    *(RSP+2)= next->regs.rdx;
    *(RSP+3)= next->regs.rcx;
    *(RSP+4)= next->regs.rbx;
    *(RSP+5)= next->regs.rax;
    *(RSP+6)= next->regs.r15;
    *(RSP+7)= next->regs.r14;
    *(RSP+8)= next->regs.r13;
    *(RSP+9)= next->regs.r12;
    *(RSP+10)= next->regs.r11;
    *(RSP+11)= next->regs.r10;
    *(RSP+12)= next->regs.r9;
    *(RSP+13)= next->regs.r8;

    schedule_context(next);
    next->state=RUNNING;
    return;
}


 static void save_sleep_context( struct exec_context* curr, struct exec_context* next, u64* RBP ) {

  /*Your code goes in here*/ 
    curr->regs.rbx = *(RBP + 15);
    curr->regs.rcx = *(RBP + 14);
    curr->regs.rdx = *(RBP + 13);
    curr->regs.rsi = *(RBP + 12);
    curr->regs.rdi = *(RBP + 11);
    curr->regs.rbp = *(RBP + 10);
    curr->regs.r8 = *(RBP + 9);
    curr->regs.r9 = *(RBP + 8);
    curr->regs.r10 = *(RBP + 7);
    curr->regs.r11 = *(RBP + 6);
    curr->regs.r12 = *(RBP + 5);
    curr->regs.r13 = *(RBP + 4);
    curr->regs.r14 = *(RBP + 3);
    curr->regs.r15 = *(RBP + 2);

    curr->regs.entry_rip = *(RBP + 16);
    curr->regs.entry_cs = *(RBP + 17);
    curr->regs.entry_rflags = *(RBP + 18);
    curr->regs.entry_rsp = *(RBP + 19);
    curr->regs.entry_ss = *(RBP + 20);


    *(RBP+16)=next->regs.entry_rip;
    *(RBP+17)=next->regs.entry_cs;
    *(RBP+18)=next->regs.entry_rflags;
    *(RBP+19)=next->regs.entry_rsp;
    *(RBP+20)=next->regs.entry_ss;


    *(RBP+15)=next->regs.rax;
    *(RBP+14)=next->regs.rbx;
    *(RBP+13)=next->regs.rcx;
    *(RBP+12)=next->regs.rdx;
    *(RBP+11)=next->regs.rsi;
    *(RBP+10)=next->regs.rdi;
    *(RBP+9)=next->regs.rbp;
    *(RBP+8)=next->regs.r8;
    *(RBP+7)=next->regs.r9;
    *(RBP+6)=next->regs.r10;
    *(RBP+5)=next->regs.r11;
    *(RBP+4)=next->regs.r12;
    *(RBP+3)=next->regs.r13;
    *(RBP+2)=next->regs.r14;
    *(RBP+1)=next->regs.r15;
next->state=RUNNING;
    schedule_context(next);
    


    return;
 } 


static struct exec_context *pick_next_context(struct exec_context *list)
{

  /*Your code goes in here*/
  
  struct exec_context *current = get_current_ctx();
  u32 curr= current->pid;

  for( int iter=1; iter<curr; iter++ ){
      if( list[iter].state== READY ){
        return list+iter;
      }
  }

  for( int iter=curr+1; iter<MAX_PROCESSES; iter++ ){
      if( list[iter].state== READY ){
        return list+iter;
      }
  }

  // Return the Swapper Process as no other process READY yet
  return list+0;

}


/*The five functions above are just a template. You may change the signatures as you wish*/
void handle_timer_tick()
{
 /*
   This is the timer interrupt handler. 
   You should account timer ticks for alarm and sleep
   and invoke schedule
 */

    // Saving the general purpose registers
    asm volatile (
         "push %%r8;"
         "push %%r9;"
         "push %%r10;"
         "push %%r11;"
         "push %%r12;"
         "push %%r13;"
         "push %%r14;"
         "push %%r15;"
         "push %%rax;"
         "push %%rbx;"
         "push %%rcx;"
         "push %%rdx;"
         "push %%rsi;"
         "push %%rdi;"
          :::"memory"  );

    // rsp
    u64 *RSP;
    asm volatile ("mov %%rsp, %0;" : "=r"( RSP )::"memory" );    

    // Obtaining rbp, error, rip
    u64 *RBP;
    asm volatile ("mov %%rbp, %0;" : "=r"( RBP )::"memory" );

    printf("Got a tick. #ticks = %u\n", ++numticks);   /*XXX Do not modify this line*/ 
    
    struct exec_context *current = get_current_ctx();
    struct exec_context *context_list = get_ctx_list(); 

    // Alarm Counter Decrement
    if( current->ticks_to_alarm > 1  && current->state==RUNNING ){
        current->ticks_to_alarm-=1;
    }
    else if( current->ticks_to_alarm == 1  && current->state==RUNNING ){

        u64* RIP= (RBP+1);
        u64* User_RSP= (RBP+4);
        // Alarm Signal Call
        current->ticks_to_alarm=0;
        invoke_sync_signal(SIGALRM, User_RSP, RIP);   
        current->ticks_to_alarm= current->alarm_config_time;
    }


    int Status=0;
    // Reducing the sleep and ticks 
    for( int i=0; i<MAX_PROCESSES; i++ ){

        // Sleep Counter Decrement
        if( context_list[i].ticks_to_sleep > 1 ){
            context_list[i].ticks_to_sleep-=1;
        }
        else if( context_list[i].ticks_to_sleep == 1 ){
            context_list[i].ticks_to_sleep=0;
            context_list[i].state=READY;      // Sleep Time Over : Move to Ready State from Waiting State
        }

        // Whether a process ready to be scheduled 
        if( context_list[i].state ==  READY ){
            // Not the swap process
            if( i!=0 && i!=current->pid ){
              Status=1;
            }
        }

    }

    // Context switch as per Round Robin Here
    if( Status ){
        struct exec_context *next= pick_next_context( get_ctx_list() );
        
        current->state = READY;
        // Context Switch Register save and restore
        save_timer_context(current, next, RBP, RSP );

    }

    ack_irq();  //acknowledge the interrupt, next interrupt     
    // printf("Heloooooooo Again\n");

    asm volatile ( "mov %0, %%rsp;"
          "pop %%rdi;"
          "pop %%rsi;"
          "pop %%rdx;"
          "pop %%rcx;"
          "pop %%rbx;"
          "pop %%rax;"
          "pop %%r15;"
          "pop %%r14;"
          "pop %%r13;"
          "pop %%r12;"
          "pop %%r11;"
          "pop %%r10;"
          "pop %%r9;"
          "pop %%r8;"
          "mov %%rbp, %%rsp;"
          "pop %%rbp;"
          "iretq;"
          : 
          :"r" (RSP)
          :"memory"
      );


    // asm volatile("mov %%rbp, %%rsp;"
    //              "pop %%rbp;"
    //              "iretq;"
    //              :::"memory");

}

void do_exit()
{
  /*You may need to invoke the scheduler from here if there are
    other processes except swapper in the system. Make sure you make 
    the status of the current process to UNUSED before scheduling 
    the next process. If the only process alive in system is swapper, 
    invoke do_cleanup() to shutdown gem5 (by crashing it, huh!)
    */

    struct exec_context *current = get_current_ctx();
    struct exec_context *context_list = get_ctx_list(); 
    current->state=UNUSED;    
    int Valid=0;

    for(int i=1; i<MAX_PROCESSES; i++){
      if( context_list[i].state != UNUSED ){
        Valid=1;
      }
    }

    if( Valid==0 ){
      do_cleanup();  /*Call this conditionally, see comments above*/
    }
    else{
      
      u64* RBP;
    asm volatile ("mov (%%rbp), %0;" :"=r" ( RBP )::"memory");
    
    struct exec_context *next= pick_next_context( get_ctx_list() );

    // Schedule
    save_sleep_context(current, next, RBP);

    // Free the OS stack of A
    asm volatile ( "mov %0, %%rsp;"
          "pop %%r15;"
          "pop %%r14;"
          "pop %%r13;"
          "pop %%r12;"
          "pop %%r11;"
          "pop %%r10;"
          "pop %%r9;"
          "pop %%r8;"
          "pop %%rbp;"
          "pop %%rdi;"
          "pop %%rsi;"
          "pop %%rdx;"
          "pop %%rcx;"
          "pop %%rbx;"
          "pop %%rax;"
          "iretq;"
          : 
          :"r" (RBP+1)
          :"memory"
      );

        // schedule_context(next);
        // next->state = RUNNING;
        // // Free the curr process OS Stack
        // save_exit_context(current, next);        
    }

}

/*system call handler for sleep*/
long do_sleep(u32 ticks)
{
    struct exec_context *current = get_current_ctx();
    current->ticks_to_sleep= ticks;
    current->state=WAITING;

    u64* RBP;
    asm volatile ("mov (%%rbp), %0;" :"=r" ( RBP )::"memory");
    // for(int i=0;i<21;i++)
    // {
    //    printf("bp - %d -> %x\n", i, *(RBP+i));
    // }

    struct exec_context *next= pick_next_context( get_ctx_list() );

    // Schedule
    save_sleep_context(current, next, RBP);

    // Free the OS stack of A
    asm volatile ( "mov %0, %%rsp;"
          "pop %%r15;"
          "pop %%r14;"
          "pop %%r13;"
          "pop %%r12;"
          "pop %%r11;"
          "pop %%r10;"
          "pop %%r9;"
          "pop %%r8;"
          "pop %%rbp;"
          "pop %%rdi;"
          "pop %%rsi;"
          "pop %%rdx;"
          "pop %%rcx;"
          "pop %%rbx;"
          "pop %%rax;"
          "iretq;"
          : 
          :"r" (RBP+1)
          :"memory"
      );

    return ticks;
}

/*
  system call handler for clone, create thread like 
  execution contexts
*/
long do_clone(void *th_func, void *user_stack)
{
    struct exec_context *parent = get_current_ctx();
    struct exec_context *child = get_new_ctx();
    u64* RBP;
    asm volatile ("mov (%%rbp), %0;" :"=r" ( RBP )::"memory");

    child->os_stack_pfn= os_pfn_alloc(OS_PT_REG);

    // Copying name from parent
    memcpy(child->name, parent->name, strlen(parent->name)-1);
    child->name[ strlen(child->name) - 1] = '0'+child->pid;
    child->name[ strlen(child->name) ]=0;

    // Child executes th_func in user_stack 
    child->regs.entry_rip = (u64)th_func;
    child->regs.entry_cs = 0x23;
    child->regs.entry_rflags = *(RBP+18);
    child->regs.entry_rsp = (u64)user_stack;
    child->regs.entry_ss = 0x2b;

    // Gen Purpose Register Copy
    child->regs.rdi= parent->regs.rdi;
    child->regs.rsi= parent->regs.rsi;
    child->regs.rdx= parent->regs.rdx;
    child->regs.rcx= parent->regs.rcx;
    child->regs.rbx= parent->regs.rbx;
    child->regs.rax= parent->regs.rax;
    
    child->regs.r15= parent->regs.r15;
    child->regs.r14= parent->regs.r14;
    child->regs.r13= parent->regs.r13;
    child->regs.r12= parent->regs.r12;
    child->regs.r11= parent->regs.r11;
    child->regs.r10= parent->regs.r10;
    child->regs.r9= parent->regs.r9;
    child->regs.r8= parent->regs.r8;


    // Same memory segements and sig handlers as of parent
    for(int i=0; i<MAX_MM_SEGS; i++){
        child->mms[i]= parent->mms[i];
    }

    for(int i=0; i<MAX_SIGNALS; i++){
        child->sighandlers[i]= parent->sighandlers[i];
    }

    // Other initialisations
    child->state= READY;
    child->pgd = parent->pgd;
    child->used_mem = parent->used_mem;
    child->ticks_to_alarm = 0;
    child->alarm_config_time = 0;
    child->ticks_to_sleep = 0;
    child->pending_signal_bitmap = 0;

    return child->pid;
   
}

long invoke_sync_signal(int signo, u64 *ustackp, u64 *urip)
{
   /*If signal handler is registered, manipulate user stack and RIP to execute signal handler*/
   /*ustackp and urip are pointers to user RSP and user RIP in the exception/interrupt stack*/
   printf("Called signal with ustackp=%x urip=%x\n", *ustackp, *urip);
   /*Default behavior is exit( ) if sighandler is not registered for SIGFPE or SIGSEGV.
    Ignore for SIGALRM*/
    
    struct exec_context *current = get_current_ctx();
    u64* handler= current->sighandlers[signo];

    if( handler==NULL ){
      // Ignore the case of SIGALRM when no handler is present
      if(signo != SIGALRM){        
        do_exit();
      }
    }
    else{

      *ustackp-=8;
      *(u64 *)*ustackp= *urip;
      *urip= (u64)handler;

    }


}
/*system call handler for signal, to register a handler*/
long do_signal(int signo, unsigned long handler)
{
    struct exec_context *current = get_current_ctx();
    u64 prev= current->sighandlers[signo];
    current->sighandlers[signo]= handler;
    return prev;
   
}

/*system call handler for alarm*/
long do_alarm(u32 ticks)
{

    struct exec_context *current = get_current_ctx();
    u32 prev= current->ticks_to_alarm;
    current->ticks_to_alarm= ticks;
    current->alarm_config_time= ticks; // Doubt
    return prev;
}
