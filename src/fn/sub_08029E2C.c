// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u32 gUnk_03005308;extern u16 gUnk_0203B400[];
int sub_08029E2C(u8*p){int start,i,index;gUnk_03005308=(gUnk_03005308+1)&1023;{register u32 offset asm("r0")=gUnk_03005308*2;asm("":"+r"(offset));{register int value asm("r4")=*(u16*)((u8*)gUnk_0203B400+offset);asm("":"+r"(value));value&=3;asm("":"+r"(value));start=value;}}for(i=start;i<start+4;i++){index=i&3;if(!((p[0x1a3]>>index)&1)){p[0x1a3]|=1<<index;goto done;}}index=start;{register int mask asm("r0")=1<<index;asm("":"+r"(mask));p[0x1a3]=mask;}done:return index+4;}
