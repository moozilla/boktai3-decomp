// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObjects_ScriptPromote sub_0801EA70
u8 *sub_0821A2C4(int);int Script_SeekToKeyword(int);int Script_GetValue(void);void sub_0801EFDC(void);
void SceneObjects_ScriptPromote(void){u8*p=sub_0821A2C4(0x85e6);int id;register int i asm("r3");if(!p)return;if(!Script_SeekToKeyword(0x6e))return;id=Script_GetValue();asm("" : "+r"(id));i=0;if(i<*(u16*)(p+0x1a)){register u8 **base asm("r6")=(u8**)(p+0x42c);register int zero asm("r5");register int two asm("r8");asm("" : : "r"(base));zero=0;asm("" : "+r"(zero));two=2;asm("" : "+r"(two));do{u8*s=*base+i*140;if(*(u16*)(s+0x34)==id){s[0x41]=1;{register int offset asm("r0")=p[0x1c]*2;register u8 *ids asm("r1");asm("" : "+r"(offset));ids=p;asm("" : "+r"(ids) : "r"(offset));ids+=0x1e;asm("" : "+r"(ids));ids+=offset;asm("" : "+r"(ids));*(u16*)ids=*(u16*)(s+0x34);}p[0x1c]++;s[0x37]=two;*(void**)(s+0x30)=sub_0801EFDC;s[0x39]=zero;s[0x3a]=zero;}i++;}while(i<*(u16*)(p+0x1a));}}
