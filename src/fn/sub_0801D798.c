#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define ResultTutorial_Press sub_0801D798
void sub_0801CAF0(void*);void sub_0801C154(void*);
void ResultTutorial_Press(u8*p,int pressed){int state=(s8)p[0xb0];asm("" : "+r"(state));if(state==0&&pressed){p[0xb0]=1;p[0xb1]=1;sub_0801CAF0(p);{register u32 v asm("r1")=*(u16*)(p+0x46);asm("" : "+r"(v));p[0xc0]=v;}sub_0801C154(p);}}
