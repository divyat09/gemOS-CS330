#include<context.h>
#include<memory.h>
#include<lib.h>

void ReLaunch( struct exec_context *ctx, u32 PTP ){
	u64*Add= osmap(PTP);
	u64 Iter;
	for(Iter=0; Iter<512; Iter++){
		*(Add+Iter)= 0;
	}
	return;
} 

void FreePTP( struct exec_context *ctx, u32 PTP_Code, u32 PTP_Data, u32 PTP_Stack ){

	u64* VirtualAdd;
	if( ( PTP_Code == PTP_Data ) && (  PTP_Code == PTP_Stack ) ){
		os_pfn_free( OS_PT_REG, PTP_Code ); 
		// printf("Clear %x %x %x \n", PTP_Code, PTP_Data, PTP_Stack );
	}
	else if( PTP_Code == PTP_Data ) { 
		os_pfn_free( OS_PT_REG, PTP_Code ); 	
		os_pfn_free( OS_PT_REG, PTP_Stack ); 	
		// printf("Clear_1 %x %x %x \n", PTP_Code, PTP_Data, PTP_Stack );
	}
	else if( PTP_Code == PTP_Stack ){
		os_pfn_free( OS_PT_REG, PTP_Code ); 	
		os_pfn_free( OS_PT_REG, PTP_Data ); 	
		// printf("Clear_2 %x %x %x \n", PTP_Code, PTP_Data, PTP_Stack );
	}
	else{
		VirtualAdd= osmap(PTP_Code);
		os_pfn_free( OS_PT_REG, PTP_Code ); 	
		VirtualAdd= osmap(PTP_Data);
		os_pfn_free( OS_PT_REG, PTP_Data ); 	
		VirtualAdd= osmap(PTP_Stack);
		os_pfn_free( OS_PT_REG, PTP_Stack ); 	
		// printf("Clear_3 %x %x %x \n", PTP_Code, PTP_Data, PTP_Stack );
	}

	return;

}

void prepare_context_mm(struct exec_context *ctx)
{
	
	//Debugging
	// printf("%x\n", ctx->mms[MM_SEG_CODE].access_flags);
	// printf("%x\n", ctx->mms[MM_SEG_DATA].access_flags);
	// printf("%x\n", ctx->mms[MM_SEG_STACK].access_flags);

	// Start Virtual Address of different segments   
	u64 StartCode= ctx->mms[MM_SEG_CODE].start;
	u64 StartData= ctx->mms[MM_SEG_DATA].start;
	u64 StartStack= ctx->mms[MM_SEG_STACK].end-0x1000;

	// Access Flags
	u32 access_flags;
	u32 permission;

	// Physical Pages for different segments
	u32 DTP_Code= os_pfn_alloc(USER_REG);
	u32 DTP_Stack= os_pfn_alloc(USER_REG);
	u32 DTP_Data= ctx->arg_pfn;

	// Page Table pages for different segments
	u32 PTP_L4= os_pfn_alloc(OS_PT_REG);
	ReLaunch( ctx, PTP_L4 );
	u32 PTP_L3= os_pfn_alloc(OS_PT_REG);
	ReLaunch( ctx,PTP_L3 );
	u32 PTP_L2= os_pfn_alloc(OS_PT_REG);
	ReLaunch( ctx,PTP_L2 );
	u32 PTP_L1= os_pfn_alloc(OS_PT_REG);
	ReLaunch( ctx, PTP_L1 );

	// Calucalting the Offset for Code Segment
	u64 Off_L4= ( StartCode & ( (1 << 48) - 1  ) ) >> 39;
	u64 Off_L3= ( StartCode & ( (1 << 39) - 1  ) ) >> 30;
	u64 Off_L2= ( StartCode & ( (1 << 30) - 1  ) ) >> 21;
	u64 Off_L1= ( StartCode & ( (1 << 21) - 1  ) ) >> 12;

	// CR3: Physical Frame Number of the first level translation
	ctx->pgd= PTP_L4;

	// Allocating pages for Code
	access_flags= ctx->mms[MM_SEG_CODE].access_flags;
	if( ((access_flags & 1) == 1 ) && ( ( (access_flags >> 1) & 1 ) == 0 ) ){
		permission= 5;
	}
	else if( ( (access_flags >> 1) & 1 ) == 1 ){
		permission= 7;
	}

	u64* VirtualAdd_L4= osmap(PTP_L4);	
	*( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;

	u64* VirtualAdd_L3= osmap(PTP_L3);
	*( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;

	u64* VirtualAdd_L2= osmap(PTP_L2);
	*( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;

	u64* VirtualAdd_L1= osmap(PTP_L1);
	*( VirtualAdd_L1 + Off_L1 )= ( ( DTP_Code << 12 ) | permission );

	// printf("Here Code L4 %x %x %x \n", PTP_L3, Off_L4, *( VirtualAdd_L4 + Off_L4 ) );
	// printf("Here Code L3 %x %x %x \n", PTP_L2, Off_L3, *( VirtualAdd_L3 + Off_L3 ) );
	// printf("Here Code L2 %x %x %x \n", PTP_L1, Off_L2, *( VirtualAdd_L2 + Off_L2 ) );
	// printf("Here Code L1 %x %x %x \n", DTP_Code, Off_L1, *( VirtualAdd_L1 + Off_L1 ) );


	// Calucalting the Offset for Data Segment
	Off_L4= ( StartData & ( (1 << 48) - 1  ) ) >> 39;
	Off_L3= ( StartData & ( (1 << 39) - 1  ) ) >> 30;
	Off_L2= ( StartData & ( (1 << 30) - 1  ) ) >> 21;
	Off_L1= ( StartData & ( (1 << 21) - 1  ) ) >> 12;


	// Allocating pages for Data
	access_flags= ctx->mms[MM_SEG_DATA].access_flags;
	if( ((access_flags & 1) == 1 ) && ( ( (access_flags >> 1) & 1 ) == 0 ) ){
		permission= 5;
	}
	else if( ( (access_flags >> 1) & 1 ) == 1 ){
		permission= 7;
	}

	// L4
	if( (u64)(*( VirtualAdd_L4 + Off_L4 ) & 1) == 0 ){
		PTP_L3 = os_pfn_alloc(OS_PT_REG);
		ReLaunch( ctx, PTP_L3 );
		VirtualAdd_L3= osmap( PTP_L3 ); 
		*( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
	}
	else{
		PTP_L3 = ( (*(VirtualAdd_L4 + Off_L4)) << 12 ) >> 24;
		VirtualAdd_L3= osmap( PTP_L3 ); 
		*( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
	}

	//L3
	if( (u64) (*( VirtualAdd_L3 + Off_L3 ) & 1) == 0 ){
		PTP_L2 = os_pfn_alloc(OS_PT_REG);
		ReLaunch( ctx, PTP_L2 );
		VirtualAdd_L2= osmap( PTP_L2 ); 
		*( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
	}
	else{
		PTP_L2 = ( (*(VirtualAdd_L3 + Off_L3)) << 12 ) >> 24;
		VirtualAdd_L2= osmap( PTP_L2 ); 
		*( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
	}


	//L2
	if( (u64)(*( VirtualAdd_L2 + Off_L2 ) & 1) == 0 ){
		PTP_L1 = os_pfn_alloc(OS_PT_REG);
		ReLaunch( ctx, PTP_L1 );
		VirtualAdd_L1= osmap( PTP_L1 ); 
		*( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
	}
	else{
		PTP_L1 = ( (*(VirtualAdd_L2 + Off_L2)) << 12 ) >> 24;
		VirtualAdd_L1= osmap( PTP_L1 ); 
		*( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
	}

	//L1: Using arg_pfn since its for Data page
	*( VirtualAdd_L1 + Off_L1 )= ( ( DTP_Data << 12 ) | permission );


	// printf("Here Data L4 %x %x %x \n", PTP_L3, Off_L4, *( VirtualAdd_L4 + Off_L4 ) );
	// printf("Here Data L3 %x %x %x \n", PTP_L2, Off_L3, *( VirtualAdd_L3 + Off_L3 ) );
	// printf("Here Data L2 %x %x %x \n", PTP_L1, Off_L2, *( VirtualAdd_L2 + Off_L2 ) );
	// printf("Here Data L1 %x %x %x \n", DTP_Data, Off_L1, *( VirtualAdd_L1 + Off_L1 ) );

	// Calucalting the Offset for Stack Segment
	Off_L4= ( StartStack & ( (1 << 48) - 1  ) ) >> 39;
	Off_L3= ( StartStack & ( (1 << 39) - 1  ) ) >> 30;
	Off_L2= ( StartStack & ( (1 << 30) - 1  ) ) >> 21;
	Off_L1= ( StartStack & ( (1 << 21) - 1  ) ) >> 12;

	// Allocating pages for Stack
	access_flags= ctx->mms[MM_SEG_STACK].access_flags;
	if( ((access_flags & 1) == 1 ) && ( ( (access_flags >> 1) & 1 ) == 0 ) ){
		permission= 5;
	}
	else if( ( (access_flags >> 1) & 1 ) == 1 ){
		permission= 7;
	}

	// L4
	if( (u64)(*( VirtualAdd_L4 + Off_L4 ) & 1) == 0 ){
		PTP_L3 = os_pfn_alloc(OS_PT_REG);
		ReLaunch( ctx, PTP_L3 );
		VirtualAdd_L3= osmap( PTP_L3 ); 
		*( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
	}
	else{
		PTP_L3 = ( (*(VirtualAdd_L4 + Off_L4)) << 12 ) >> 24;
		VirtualAdd_L3= osmap( PTP_L3 ); 
		*( VirtualAdd_L4 + Off_L4 )= ( PTP_L3 << 12 ) | permission ;
	}

	//L3
	if( (u64)(*( VirtualAdd_L3 + Off_L3 ) & 1) == 0 ){
		PTP_L2 = os_pfn_alloc(OS_PT_REG);
		ReLaunch( ctx, PTP_L2 );
		VirtualAdd_L2= osmap( PTP_L2 ); 
		*( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
	}
	else{
		PTP_L2 = ( (*(VirtualAdd_L3 + Off_L3)) << 12 ) >> 24;
		VirtualAdd_L2= osmap( PTP_L2 ); 
		*( VirtualAdd_L3 + Off_L3 )= ( PTP_L2 << 12 ) | permission ;
	}


	//L2
	if( (u64)(*( VirtualAdd_L2 + Off_L2 ) & 1) == 0 ){
		PTP_L1 = os_pfn_alloc(OS_PT_REG);
		ReLaunch( ctx, PTP_L1 );
		VirtualAdd_L1= osmap( PTP_L1 ); 
		*( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
	}
	else{
		PTP_L1 = ( (*(VirtualAdd_L2 + Off_L2)) << 12 ) >> 24;
		VirtualAdd_L1= osmap( PTP_L1 ); 
		*( VirtualAdd_L2 + Off_L2 )= ( PTP_L1 << 12 ) | permission ;
	}

	//L1: Using arg_pfn since its for Data page
	*( VirtualAdd_L1 + Off_L1 )= ( ( DTP_Stack << 12 ) | permission );

	// printf("Here Stack L4 %x %x %x \n", PTP_L3, Off_L4, *( VirtualAdd_L4 + Off_L4 ) );
	// printf("Here Stack L3 %x %x %x \n", PTP_L2, Off_L3, *( VirtualAdd_L3 + Off_L3 ) );
	// printf("Here Stack L2 %x %x %x \n", PTP_L1, Off_L2, *( VirtualAdd_L2 + Off_L2 ) );
	// printf("Here Stack L1 %x %x %x \n", DTP_Stack, Off_L1, *( VirtualAdd_L1 + Off_L1 ) );

   return;
}

void cleanup_context_mm(struct exec_context *ctx)
{


	//Start Virtual Address of different segments   
	u64 StartCode= ctx->mms[MM_SEG_CODE].start;
	u64 StartData= ctx->mms[MM_SEG_DATA].start;
	u64 StartStack= ctx->mms[MM_SEG_STACK].end-1;

	// Initialization
	u32 PTP_L4 = ctx->pgd;
	u32 PTP_L3_Code;
	u32 PTP_L3_Data;
	u32 PTP_L3_Stack;
	u32 PTP_L2_Code;
	u32 PTP_L2_Data;
	u32 PTP_L2_Stack;
	u32 PTP_L1_Code;
	u32 PTP_L1_Data;
	u32 PTP_L1_Stack;

	// Calucalting the Offset for Code Segment
	u64 Off_L4= ( StartCode & ( (1 << 48) - 1  ) ) >> 39;
	u64 Off_L3= ( StartCode & ( (1 << 39) - 1  ) ) >> 30;
	u64 Off_L2= ( StartCode & ( (1 << 30) - 1  ) ) >> 21;
	u64 Off_L1= ( StartCode & ( (1 << 21) - 1  ) ) >> 12;
	u64* VirtualAdd= osmap(PTP_L4);


	// Physical Page Numbers for Code Segment
	PTP_L3_Code = ( (*(VirtualAdd + Off_L4)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L3_Code);

	PTP_L2_Code = ( (*(VirtualAdd + Off_L3)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L2_Code);

	PTP_L1_Code = ( (*(VirtualAdd + Off_L2)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L1_Code);

	u32 DTP_Code= ( (*(VirtualAdd + Off_L1)) << 12 ) >> 24;


	// Calucalting the Offset for Data Segment
	Off_L4= ( StartData & ( (1 << 48) - 1  ) ) >> 39;
	Off_L3= ( StartData & ( (1 << 39) - 1  ) ) >> 30;
	Off_L2= ( StartData & ( (1 << 30) - 1  ) ) >> 21;
	Off_L1= ( StartData & ( (1 << 21) - 1  ) ) >> 12;
	VirtualAdd= osmap(PTP_L4);


	// Physical Page Numbers for Data Segment
	PTP_L3_Data = ( (*(VirtualAdd + Off_L4)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L3_Data);

	PTP_L2_Data = ( (*(VirtualAdd + Off_L3)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L2_Data);

	PTP_L1_Data = ( (*(VirtualAdd + Off_L2)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L1_Data);

	u32 DTP_Data= ( (*(VirtualAdd + Off_L1)) << 12 ) >> 24;


	// Calucalting the Offset for Stack Segment
	Off_L4= ( StartStack & ( (1 << 48) - 1  ) ) >> 39;
	Off_L3= ( StartStack & ( (1 << 39) - 1  ) ) >> 30;
	Off_L2= ( StartStack & ( (1 << 30) - 1  ) ) >> 21;
	Off_L1= ( StartStack & ( (1 << 21) - 1  ) ) >> 12;
	VirtualAdd= osmap(PTP_L4);


	// Physical Page Numbers for Stack Segment
	PTP_L3_Stack = ( (*(VirtualAdd + Off_L4)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L3_Stack);

	PTP_L2_Stack = ( (*(VirtualAdd + Off_L3)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L2_Stack);

	PTP_L1_Stack = ( (*(VirtualAdd + Off_L2)) << 12 ) >> 24;
	VirtualAdd= osmap(PTP_L1_Stack);

	u32 DTP_Stack= ( (*(VirtualAdd + Off_L1)) << 12 ) >> 24;

	// Free Page Table Pages at Level L1, L2 and L3
	FreePTP( ctx, PTP_L1_Code, PTP_L1_Data, PTP_L1_Stack );
	FreePTP( ctx, PTP_L2_Code, PTP_L2_Data, PTP_L2_Stack );
	FreePTP( ctx, PTP_L3_Code, PTP_L3_Data, PTP_L3_Stack );

	// Free L3 page
	VirtualAdd= osmap(PTP_L4);
	os_pfn_free( OS_PT_REG, PTP_L4 ); 

	// Free Physical Page
	os_pfn_free( USER_REG, DTP_Code ); 
	os_pfn_free( USER_REG, DTP_Data ); 
	os_pfn_free( USER_REG, DTP_Stack ); 

   return;
}
