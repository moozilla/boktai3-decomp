#include "global.h"
s32 Mod(s32,s32);
void sub_0820B034(void*);
void sub_08220C8C(void*,void*,u32,u32,u32);
void sub_0820B258(void);
u32 sub_0820B174(u8 *p)
{
 if(*(u32*)(p+0x164)) {
  s16 *index=(s16*)(p+0x186);
  if(*(s32*)(p+0x18) > ((s16*)(p+0x188))[*index]) {
   u32 zero=0;
   sub_0820B034(p);
   sub_08220C8C(p+0x10c,*(void**)(p+0x170),4,p[0x132],0);
   *(u16*)(p+0x12c)=30;
   *(void**)(p+0x108)=sub_0820B258;
   *(u32*)(p+0x18)=zero;
   (*index)++;
   if(*index > *(s16*)(p+0x184)-1) *index=zero;
  }
 } else {
  s32 value=Mod(*(s32*)(p+0x18),*(s16*)(p+0x134));
  if(!value) {
   sub_0820B034(p);
   sub_08220C8C(p+0x10c,*(void**)(p+0x170),4,p[0x132],value);
   *(u16*)(p+0x12c)=30;
   *(u32*)(p+0x18)=value;
   *(void**)(p+0x108)=sub_0820B258;
  }
 }
}
