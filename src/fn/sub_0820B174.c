#include "global.h"
s32 Mod(s32,s32);
void sub_0820B034(void*);
void sub_08220C8C(void*,void*,u32,u32,u32);
void sub_0820B258(void);
u32 sub_0820B174(u8 *p)
{
 if(*(u32*)(p+0x164)) {
  s16 *index=(s16*)(p+0x186);
  s32 selected=*index*2;
  s16 *table=(s16*)(p+0x188);
  if(*(s32*)(p+0x18) > *(s16*)((u8*)table+selected)) {
   u32 zero;
   sub_0820B034(p);
   {void *obj=p+0x10c;void *res=*(void**)(p+0x170);u32 value=p[0x132];zero=0;
   sub_08220C8C(obj,res,4,value,zero);}
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
