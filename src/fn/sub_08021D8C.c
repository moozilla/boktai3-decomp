// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneActors_SpawnAmbientParticle sub_08021D8C
extern u16 gUnk_0203B400[];extern u32 gUnk_03005308;void sub_08015C30(void*,int,int,int,int,int,int,int);
void SceneActors_SpawnAmbientParticle(u8*p){struct{s16 x,y,z;}position;register int r asm("r3");register int cursor asm("r1");register u16 *table asm("r6")=gUnk_0203B400;asm("" : "+r"(table));cursor=gUnk_03005308+1;{register int mask asm("r4")=0x3ff;asm("" : : "r"(mask));cursor&=0x3ff;}asm("" : "+r"(cursor));r=table[cursor]&0x7f;asm("" : "+r"(r));position.x=*(s16*)(p+0x54)+r;cursor=(cursor+1)&0x3ff;gUnk_03005308=cursor;cursor=table[cursor];asm("" : "+r"(cursor));{u16*y=(u16*)(p+0x56);asm("" : : "r"(y));cursor&=0x1f;asm("" : "+r"(cursor));{register int height asm("r4")=*y;asm("" : "+r"(height));cursor+=height;}position.y=cursor;}{u16 *z=(u16*)(p+0x58);asm("" : : "r"(z));r-=0x7f;asm("" : "+r"(r));position.z=*z-r;}sub_08015C30(&position,1,0,0x40,12,12,4,4);}
