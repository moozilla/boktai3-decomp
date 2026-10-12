// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Packed{s32 x:16,y:16,z:16,pad:16;};extern u32 gUnk_03005308;extern u16 gUnk_0203B400[];void sub_0806A160(void*,int,void*,int,int,int,int,int);
void sub_08029EB4(u8*p){u8*graphics=p+0x888;struct Packed size;int i;for(i=0;i<2;i++){int angle,speed;register int count asm("r2");register u32 index asm("r1");u32*indexPtr;u32 mask;int smallMask;register u16*table asm("r4");size.x=164;size.y=16;size.z=164;indexPtr=&gUnk_03005308;mask=1023;smallMask=3;table=gUnk_0203B400;asm(""::"r"(table));index=(*indexPtr+1)&mask;angle=(u8)table[index];index=(index+1)&mask;count=(table[index]&smallMask)+2;index=(index+1)&mask;*indexPtr=index;speed=(table[index]&smallMask)+20;sub_0806A160(graphics+0x1c,3,&size,angle,count,speed,2,2);}}
