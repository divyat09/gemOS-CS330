#include<init.h>
#include<lib.h>
#include<context.h>
#include<memory.h>

void ReLaunch( struct exec_context *ctx, u32 PTP ){
  u64*Add= osmap(PTP);
  u64 Iter;
  for(Iter=0; Iter<512; Iter++){
    *(Add+Iter)= 0;
  }
  return;
} 

/*System Call handler*/
u64 do_syscall(int syscall, u64 param1, u64 param2, u64 param3, u64 param4)
{
    struct exec_context *current = get_current_ctx();
    printf("[GemOS] System call invoked. syscall no  = %d\n", syscall);
    switch(syscall)
    {
          case SYSCALL_EXIT:
                              printf("[GemOS] exit code = %d\n", (int) param1);
                              do_exit();
                              break;
          case SYSCALL_GETPID:
                              printf("[GemOS] getpid called for process %s, with pid = %d\n", current->name, current->id);
                              return current->id;      
          case SYSCALL_WRITE:
                             {  
                                    /*Your code goes here*/
                                    // Invalid Length Parameter
                                    if( param2 > 1024 || param2 < 0 ){
                                      // printf("Invalid Buffer Length\n");
                                      return -1;
                                    }

                                    // Validity of Buffer Address
                                    u64 Off_L4= ( param1 & ( (1 << 48) - 1  ) ) >> 39;
                                    u64 Off_L3= ( param1 & ( (1 << 39) - 1  ) ) >> 30;
                                    u64 Off_L2= ( param1 & ( (1 << 30) - 1  ) ) >> 21;
                                    u64 Off_L1= ( param1 & ( (1 << 21) - 1  ) ) >> 12;
                                    
                                    u32 PTP = current->pgd;
                                    u64* VirtualAdd= osmap(PTP);

                                    // L4
                                    if( (u64)( *( VirtualAdd + Off_L4 ) & 1 ) == 0 ){
                                      // printf("Invalid L4 Entry for param 1\n");
                                      return -1;
                                    }
                                    else{                                    
                                      PTP = ( *( VirtualAdd + Off_L4 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);
                                    }

                                    // L3
                                    if( (u64)( *( VirtualAdd + Off_L3 ) & 1) == 0 ){
                                      // printf("Invalid L3 Entry for param 1\n");
                                      return -1;
                                    }                                                              
                                    else{
                                      PTP = ( *( VirtualAdd + Off_L3 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);                                      
                                    }

                                    // L2
                                    if( (u64)( *( VirtualAdd + Off_L2 ) & 1) == 0 ){
                                      // printf("Invalid L2 Entry for param 1\n");
                                      return -1;
                                    }                                                             
                                    else{
                                      PTP = ( *( VirtualAdd + Off_L2 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);                                      
                                    } 

                                    // L1
                                    if( (u64)( *( VirtualAdd + Off_L1 ) & 1) == 0 ){
                                      // printf("Invalid L1 Entry for param 1\n");
                                      return -1;
                                    }  
                                    else{
                                      PTP = ( *( VirtualAdd + Off_L1 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);                                      
                                    }                                                            


                                    // Validity of Buffer Address End Length Part
                                    Off_L4= ( ( param1 + param2 -1 ) & ( (1 << 48) - 1  ) ) >> 39;
                                    Off_L3= ( ( param1 + param2 -1 ) & ( (1 << 39) - 1  ) ) >> 30;
                                    Off_L2= ( ( param1 + param2 -1 ) & ( (1 << 30) - 1  ) ) >> 21;
                                    Off_L1= ( ( param1 + param2 -1 ) & ( (1 << 21) - 1  ) ) >> 12;
                                    
                                    PTP = current->pgd;
                                    VirtualAdd= osmap(PTP);

                                    // L4
                                    if( (u64)( *( VirtualAdd + Off_L4 ) & 1 ) == 0 ){
                                      // printf("Invalid L4 Entry for param1+param2-1\n");
                                      return -1;
                                    }
                                    else{                                    
                                      PTP = ( *( VirtualAdd + Off_L4 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);
                                    }

                                    // L3
                                    if( (u64)( *( VirtualAdd + Off_L3 ) & 1) == 0 ){
                                      // printf("Invalid L3 Entry for param1+param2-1\n");
                                      return -1;
                                    }                                                              
                                    else{
                                      PTP = ( *( VirtualAdd + Off_L3 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);                                      
                                    }

                                    // L2
                                    if( (u64)( *( VirtualAdd + Off_L2 ) & 1) == 0 ){
                                      // printf("Invalid L2 Entry for param1+param2-1\n");
                                      return -1;
                                    }                                                             
                                    else{
                                      PTP = ( *( VirtualAdd + Off_L2 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);                                      
                                    } 

                                    // L1
                                    if( (u64)( *( VirtualAdd + Off_L1 ) & 1) == 0 ){
                                      // printf("Invalid L1 Entry for param1+param2-1\n");
                                      return -1;
                                    }  
                                    else{
                                      PTP = ( *( VirtualAdd + Off_L1 ) << 12 ) >> 24;
                                      VirtualAdd = osmap(PTP);                                      
                                    }

                                    int i=0;
                                    for(i=0;i<param2;i++){
                                       printf("%c", *((char*)param1 + i) ); 
                                    }
                                    printf("\n");                   
                                    return param2;                                         
                             }

          case SYSCALL_EXPAND:
                             {  
                                    /*Your code goes here*/
                                    // ( u32 size, int flags )

                                    if( param1 > 512 || param1 < 0 ){
                                      // printf("Invalid Size for Expand\n");
                                      return NULL;
                                    }

                                    // MM_SEG_RODATA
                                    if( param2 == MAP_RD ){

                                      if( current->mms[MM_SEG_RODATA].next_free + param1*PAGE_SIZE > current->mms[MM_SEG_RODATA].end ){
                                        // printf("Next Free exceeds End for Read Only Data\n");
                                        return NULL;
                                      }
                                      else{
                                        u64 RetVal = current->mms[MM_SEG_RODATA].next_free; 
                                        current->mms[MM_SEG_RODATA].next_free += param1*PAGE_SIZE;
                                        // printf("Next Free in Expand for Read Only Data %x\n", RetVal);
                                        return RetVal;
                                      }

                                    }

                                    // MMM_SEG_DATA
                                    else if( param2 == MAP_WR){

                                      if( current->mms[MM_SEG_DATA].next_free + param1*PAGE_SIZE > current->mms[MM_SEG_DATA].end ){
                                        // printf("Next Free exceeds End for Data\n");
                                        return NULL;
                                      }
                                      else{
                                        u64 RetVal= current->mms[MM_SEG_DATA].next_free;  
                                        current->mms[MM_SEG_DATA].next_free += param1*PAGE_SIZE;
                                        // printf("Next Free in Expand for Data %x\n", RetVal);
                                        return RetVal;
                                      }

                                    }

                             }
 
          case SYSCALL_SHRINK:
                             {  
                                    /*Your code goes here*/
                                    // ( u32 size, int flags )

                                    if( param1 < 0 ){
                                      // printf("Invalid Size for Shrink\n");
                                      return NULL;
                                    }

                                    // MM_SEG_RODATA
                                    if( param2 == MAP_RD ){

                                      if( current->mms[MM_SEG_RODATA].next_free - param1*PAGE_SIZE < current->mms[MM_SEG_RODATA].start ){
                                        // printf("Exceeds Start for Read Only Data\n");
                                        return NULL;
                                      }
                                      else{
                                        u64 i=0;
    
                                        while( i<param1 ){

                                          // Move backward by one PAGE SIZE
                                          current->mms[MM_SEG_RODATA].next_free -= PAGE_SIZE;
                                          i++;

                                          u64 Off_L4= ( current->mms[MM_SEG_RODATA].next_free & ( (1 << 48) - 1  ) ) >> 39;
                                          u64 Off_L3= ( current->mms[MM_SEG_RODATA].next_free & ( (1 << 39) - 1  ) ) >> 30;
                                          u64 Off_L2= ( current->mms[MM_SEG_RODATA].next_free & ( (1 << 30) - 1  ) ) >> 21;
                                          u64 Off_L1= ( current->mms[MM_SEG_RODATA].next_free & ( (1 << 21) - 1  ) ) >> 12;
                                          u32 PTP = current->pgd;
                                          u64* VirtualAdd= osmap(PTP);

                                          // L4 PTP
                                          // printf(" Shrink L4 Iter %x: %x %x\n", i, Off_L4, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L4 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            PTP= ( *( VirtualAdd + Off_L4 ) << 12 ) >> 24;
                                            VirtualAdd= osmap(PTP);
                                          }

                                          // L3 PTP
                                          // printf(" Shrink L3 Iter %x: %x %x\n", i, Off_L3, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L3 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            PTP= ( *( VirtualAdd + Off_L3 ) << 12 ) >> 24;
                                            VirtualAdd= osmap(PTP);
                                          }

                                          // L2 PTP
                                          // printf(" Shrink L2 Iter %x: %x %x\n", i, Off_L2, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L2 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            PTP= ( *( VirtualAdd + Off_L2 ) << 12 ) >> 24;
                                            VirtualAdd= osmap(PTP);
                                          }
                                            
                                          // L1 PTP
                                          // printf(" Shrink L1 Iter %x: %x %x\n", i, Off_L1, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L1 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            // Set the present bit in L1 to zero
                                            PTP= ( *( VirtualAdd + Off_L1 ) << 12 ) >> 24;
                                            *( VirtualAdd + Off_L1 )= ( *( VirtualAdd + Off_L1 ) >> 1 ) << 1;

                                            //Free the physical data page
                                            os_pfn_free( USER_REG, PTP );

                                            asm volatile ("invlpg (%0);"::"r"(current->mms[MM_SEG_RODATA].next_free) : "memory");

                                          }

                                        }
                                        // printf("Next Free for Shrink in Read Only Data %x\n", current->mms[MM_SEG_RODATA].next_free);
                                        return current->mms[MM_SEG_RODATA].next_free;
                                      }
                                    }

                                    // MAP_SEG_DATA
                                    else if(  param2 == MAP_WR ){


                                      if( current->mms[MM_SEG_DATA].next_free - param1*PAGE_SIZE < current->mms[MM_SEG_DATA].start ){
                                        // printf("Exceeds Start for Data\n");
                                        return NULL;
                                      }
                                      else{
                                        u64 i=0;

                                        while( i<param1 ){

                                          // Move backward by one PAGE SIZE
                                          current->mms[MM_SEG_DATA].next_free -= PAGE_SIZE;
                                          i++;

                                          u64 Off_L4= ( current->mms[MM_SEG_DATA].next_free & ( (1 << 48) - 1  ) ) >> 39;
                                          u64 Off_L3= ( current->mms[MM_SEG_DATA].next_free & ( (1 << 39) - 1  ) ) >> 30;
                                          u64 Off_L2= ( current->mms[MM_SEG_DATA].next_free & ( (1 << 30) - 1  ) ) >> 21;
                                          u64 Off_L1= ( current->mms[MM_SEG_DATA].next_free & ( (1 << 21) - 1  ) ) >> 12;
                                          u32 PTP = current->pgd;
                                          u64* VirtualAdd= osmap(PTP);

                                          // L4 PTP
                                          // printf(" Shrink L4 Iter %x: %x %x\n", i, Off_L4, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L4 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            PTP= ( *( VirtualAdd + Off_L4 ) << 12 ) >> 24;
                                            VirtualAdd= osmap(PTP);
                                          }

                                          // L3 PTP
                                          // printf(" Shrink L3 Iter %x: %x %x\n", i, Off_L3, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L3 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            PTP= ( *( VirtualAdd + Off_L3 ) << 12 ) >> 24;
                                            VirtualAdd= osmap(PTP);
                                          }

                                          // L2 PTP
                                          // printf(" Shrink L2 Iter %x: %x %x\n", i, Off_L2, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L2 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            PTP= ( *( VirtualAdd + Off_L2 ) << 12 ) >> 24;
                                            VirtualAdd= osmap(PTP);
                                          }
                                            
                                          // L1 PTP
                                          // printf(" Shrink L1 Iter %x: %x %x\n", i, Off_L1, PTP );
                                          if( (u64)( *( VirtualAdd + Off_L1 ) & 1) ==0 ){
                                            continue;
                                          }
                                          else{
                                            // Set the present bit in L1 to zero
                                            PTP= ( *( VirtualAdd + Off_L1 ) << 12 ) >> 24;
                                            *( VirtualAdd + Off_L1 )= ( *( VirtualAdd + Off_L1 ) >>  1 ) << 1;

                                            // Free the physical data page
                                            os_pfn_free( USER_REG, PTP );

                                            asm volatile ("invlpg (%0);"::"r"(current->mms[MM_SEG_DATA].next_free) : "memory");
                                          }

                                        }
                                        // printf("Next Free for Shrink in Data %x\n", current->mms[MM_SEG_DATA].next_free );
                                        return current->mms[MM_SEG_DATA].next_free;
                                      }
                                    }
                             }
                             
          default:
                            return -1;
                                
    }
    return 0;   /*GCC shut up!*/
}

extern int handle_div_by_zero(void)
{
    /*Your code goes in here*/
    struct exec_context *current = get_current_ctx();
    u64* RIP;
    // Loading Base Pointer in RIP
    asm volatile ("mov %%rbp, %0;" :"=r" ( RIP )::"memory");
    // True RIP at RBP + 8 as 64 bit for RIP
    printf( "Div-by-zero detected at [%x]", *(RIP + 1) );
    do_exit();  
    // printf("Div-by-zero handler detected at %x\n", *RIP );
    return 0;
}

extern int handle_page_fault(void)
{ 
    /*Your code goes in here*/

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
    u64 Error= *(RBP +1);
    u64 RIP= *(RBP + 2);

    // printf("Page Fault Error: \n");
    struct exec_context *current = get_current_ctx();

    // Faulting Virtual Address
    u64 FaultingVA;
    asm volatile ( "mov %%cr2, %0;" : "=r" ( FaultingVA ) : : "memory" );

    // printf("OS Stack: RSP: %x, RBP: %x, Error: %x, RIP: %x Address: %x\n", *RSP, *RBP, Error, RIP, FaultingVA );

    // Page Table Page and Data Page
    u64 Address= FaultingVA;
    u32 PTP_L4 = current->pgd;
    u64* VirtualAdd_L4= osmap(PTP_L4);
    u64* VirtualAdd_L3;
    u64* VirtualAdd_L2;
    u64* VirtualAdd_L1;
    u32 PTP_L3, PTP_L2, PTP_L1;
    u32 DTP;

    // Access Flags
    u32 access_flags;
    u32 permission;

    // Offsets
    u64 Off_L4= ( Address & ( (1 << 48) - 1  ) ) >> 39;
    u64 Off_L3= ( Address & ( (1 << 39) - 1  ) ) >> 30;
    u64 Off_L2= ( Address & ( (1 << 30) - 1  ) ) >> 21;
    u64 Off_L1= ( Address & ( (1 << 21) - 1  ) ) >> 12;

    // Checking Bit 0 of Error Code
    if( Error & 1 == 1 ){
      //Protection Violation: Bit 0 == 1
      printf("Protection Violation ");
      printf("RIP: %x, Accesed Virutal Code: %x, Error: %x\n", RIP, Address, Error  );
      do_exit();
    } 
    // else: Bit 0 == 0 : Some entry not present in PTP Case

    // Data Case
    if( ( Address >= current->mms[MM_SEG_DATA].start ) &&  ( Address <= current->mms[MM_SEG_DATA].end ) ){

      // Not Legitimate
      if( Address >= current->mms[MM_SEG_DATA].next_free ){
        printf(" Illegitimate Address in Data Segement ");
        printf("RIP: %x, Accesed Virutal Code: %x, Error: %x\n", RIP, Address, Error  );
        do_exit();
      }

      // Permission
      access_flags= current->mms[MM_SEG_DATA].access_flags;
      if( ((access_flags & 1) == 1 ) && ( ( (access_flags >> 1) & 1 ) == 0 ) ){
        permission= 5;
      }
      else if( ( (access_flags >> 1) & 1 ) == 1 ){
        permission= 7;
      }

      // L4
      // printf(" Data: %x %x\n", Off_L4, PTP_L4 );
      if( (u64)(*( VirtualAdd_L4 + Off_L4 ) & 1) == 0 ){
        PTP_L3 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L3 );
        VirtualAdd_L3= osmap( PTP_L3 ); 
        *( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
      }
      else{
        PTP_L3 = ( (*(VirtualAdd_L4 + Off_L4)) << 12 ) >> 24;
        VirtualAdd_L3= osmap( PTP_L3 ); 
        *( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
      }

      //L3
      // printf(" Data: %x %x\n", Off_L3, PTP_L3 );
      if( (u64)(*( VirtualAdd_L3 + Off_L3 ) & 1) == 0 ){
        PTP_L2 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L2 );
        VirtualAdd_L2= osmap( PTP_L2 ); 
        *( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
      }
      else{
        PTP_L2 = ( (*(VirtualAdd_L3 + Off_L3)) << 12 ) >> 24;
        VirtualAdd_L2= osmap( PTP_L2 ); 
        *( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
      }


      //L2
      // printf(" Data: %x %x\n", Off_L2, PTP_L2 );
      if( (u64)(*( VirtualAdd_L2 + Off_L2 ) & 1) == 0 ){
        PTP_L1 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L1 );
        VirtualAdd_L1= osmap( PTP_L1 ); 
        *( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
      }
      else{
        PTP_L1 = ( (*(VirtualAdd_L2 + Off_L2)) << 12 ) >> 24;
        VirtualAdd_L1= osmap( PTP_L1 ); 
        *( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
      }

      //L1
      // printf(" Data: %x %x\n", Off_L1, PTP_L1 );
      if( (u64)(*( VirtualAdd_L1 + Off_L1 ) & 1) == 0 ){
        DTP = os_pfn_alloc(USER_REG);
        *( VirtualAdd_L1 + Off_L1 )= ( DTP << 12 ) | permission ;
      }

    }

    // Read Only Data Case
    else if( ( Address >= current->mms[MM_SEG_RODATA].start ) &&  ( Address <= current->mms[MM_SEG_RODATA].end ) ){

      // Not Legitimate
      if( Address >= current->mms[MM_SEG_RODATA].next_free ){
        printf(" Illegitimate Address in Read Only Data Segement ");
        printf("RIP: %x, Accesed Virutal Code: %x, Error: %x\n", RIP, Address, Error  );
        do_exit();
      }

      // Write Bit Set: 2nd Bit of Error is 1
      if ( ( Error >> 1 ) & 1 == 1 ){
        printf("Error: Write in Read Only Data Segment ");
        printf("RIP: %x, Accesed Virutal Code: %x, Error: %x\n", RIP, Address, Error  );
        do_exit();
      }

      // Permission
      access_flags= current->mms[MM_SEG_RODATA].access_flags;
      if( ((access_flags & 1) == 1 ) && ( ( (access_flags >> 1) & 1 ) == 0 ) ){
        permission= 5;
      }
      else if( ( (access_flags >> 1) & 1 ) == 1 ){
        permission= 7;
      }

      // L4
      // printf(" RODATA: %x %x\n", Off_L4, PTP_L4 );
      if( (u64)(*( VirtualAdd_L4 + Off_L4 ) & 1) == 0 ){
        PTP_L3 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L3 );
        VirtualAdd_L3= osmap( PTP_L3 ); 
        *( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
      }
      else{
        PTP_L3 = ( (*(VirtualAdd_L4 + Off_L4)) << 12 ) >> 24;
        VirtualAdd_L3= osmap( PTP_L3 ); 
        *( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
      }

      //L3
      // printf(" RODATA: %x %x\n", Off_L3, PTP_L3 );
      if( (u64)(*( VirtualAdd_L3 + Off_L3 ) & 1) == 0 ){
        PTP_L2 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L2 );
        VirtualAdd_L2= osmap( PTP_L2 ); 
        *( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
      }
      else{
        PTP_L2 = ( (*(VirtualAdd_L3 + Off_L3)) << 12 ) >> 24;
        VirtualAdd_L2= osmap( PTP_L2 ); 
        *( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
      }


      //L2
      // printf(" RODATA: %x %x\n", Off_L2, PTP_L2 );
      if( (u64)(*( VirtualAdd_L2 + Off_L2 ) & 1) == 0 ){
        PTP_L1 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L1 );
        VirtualAdd_L1= osmap( PTP_L1 ); 
        *( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
      }
      else{
        PTP_L1 = ( (*(VirtualAdd_L2 + Off_L2)) << 12 ) >> 24;
        VirtualAdd_L1= osmap( PTP_L1 ); 
        *( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
      }

      //L1
      // printf(" RO Data: %x %x\n", Off_L1, PTP_L1 );
      if( (u64)(*( VirtualAdd_L1 + Off_L1 ) & 1) == 0 ){
        DTP = os_pfn_alloc(USER_REG);
        *( VirtualAdd_L1 + Off_L1 )= ( DTP << 12 ) | permission ;
      }

    }

    // Stack Case
    else if( ( Address >= current->mms[MM_SEG_STACK].start ) &&  ( Address <= current->mms[MM_SEG_STACK].end ) ){

      // Not Legitimate
      if( Address > current->mms[MM_SEG_STACK].end ){
        printf(" Illegitimate Address in Stack Segement ");
        printf("RIP: %x, Accesed Virutal Code: %x, Error: %x\n", RIP, Address, Error  );
        do_exit();
      }

      // Permission
      access_flags= current->mms[MM_SEG_STACK].access_flags;
      if( ((access_flags & 1) == 1 ) && ( ( (access_flags >> 1) & 1 ) == 0 ) ){
        permission= 5;
      }
      else if( ( (access_flags >> 1) & 1 ) == 1 ){
        permission= 7;
      }

      // L4
      // printf(" STACK: %x %x\n", Off_L4, PTP_L4 );
      if( (u64)(*( VirtualAdd_L4 + Off_L4 ) & 1) == 0 ){
        PTP_L3 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L3 );
        VirtualAdd_L3= osmap( PTP_L3 ); 
        *( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
      }
      else{
        PTP_L3 = ( (*(VirtualAdd_L4 + Off_L4)) << 12 ) >> 24;
        VirtualAdd_L3= osmap( PTP_L3 ); 
        *( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
      }

      //L3
      // printf(" STACK: %x %x\n", Off_L3, PTP_L3 );
      if( (u64)(*( VirtualAdd_L3 + Off_L3 ) & 1) == 0 ){
        PTP_L2 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L2 );
        VirtualAdd_L2= osmap( PTP_L2 ); 
        *( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
      }
      else{
        PTP_L2 = ( (*(VirtualAdd_L3 + Off_L3)) << 12 ) >> 24;
        VirtualAdd_L2= osmap( PTP_L2 ); 
        *( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
      }


      //L2
      // printf(" STACK: %x %x\n", Off_L2, PTP_L2 );
      if( (u64)(*( VirtualAdd_L2 + Off_L2 ) & 1) == 0 ){
        PTP_L1 = os_pfn_alloc(OS_PT_REG);
        ReLaunch( current, PTP_L1 );
        VirtualAdd_L1= osmap( PTP_L1 ); 
        *( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
      }
      else{
        PTP_L1 = ( (*(VirtualAdd_L2 + Off_L2)) << 12 ) >> 24;
        VirtualAdd_L1= osmap( PTP_L1 ); 
        *( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
      }

      //L1
      // printf(" Stack: %x %x\n", Off_L1, PTP_L1 );
      if( (u64)(*( VirtualAdd_L1 + Off_L1 ) & 1) == 0 ){
        DTP = os_pfn_alloc(USER_REG);
        *( VirtualAdd_L1 + Off_L1 )= ( DTP << 12 ) | permission ;
      }

    }

    // Not a valid virtual memory segment
    else{
      printf(" Illegitimate Address, Not in any Segment ");
      printf("RIP: %x, Accesed Virutal Code: %x, Error: %x\n", RIP, Address, Error  );
      do_exit();
    }

    // printf("Done :P \n");
    // printf("OS Stack 2: RSP: %x, RBP: %x, Error: %x, RIP: %x, Address: %x\n", *RSP, *RBP, Error, RIP, FaultingVA );

    // printf("Page Fault Handled: \n");
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
          "add $8, %%rsp;"
          "iretq;"
          : 
          :"r" (RSP)
          :"memory"
      );
    return ;
}
