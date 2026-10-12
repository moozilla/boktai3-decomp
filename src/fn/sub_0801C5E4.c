// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define ResultMenu_Open sub_0801C5E4
int sub_0801B54C(void*);int sub_0801B49C(int,int);void sub_0801BD44(void*);void sub_082279A8(int,int,int,int,int,int,int);extern void *gUnknown_02000580;void sub_0815BE5C(void*,int);void sub_0801B534(void*,void*);void sub_0801C650(void*);
void ResultMenu_Open(u8*p){if(sub_0801B54C(p)){int layer=sub_0801B49C(14,0);u8 *out=p+0xb7;int zero;asm("" : "+r"(out));zero=0;asm("" : "+r"(zero));*out=layer;sub_0801BD44(p);sub_082279A8(0,5,4,4,4,0xffff,zero);sub_0815BE5C(gUnknown_02000580,0);}if(*(u32*)(p+0x3c)>31)sub_0801B534(p,sub_0801C650);else ++*(u32*)(p+0x3c);}
