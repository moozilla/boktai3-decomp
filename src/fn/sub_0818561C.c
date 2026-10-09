#include "global.h"
extern u8 *gUnk_02000710;
struct P {u32 x:16,y:16,z:16,w:16;};
s32 sub_082215E4(s32,s32);void sub_081850F4(void *);s32 sub_0818505C(void *,s32);
void sub_0818561C(u8 *p){
 u8 *state=p+0xb3d;
 switch(*state){
 case 0:{struct P v;u8 *actor=gUnk_02000710;s32 angle;
 v.x=*(u16 *)(actor+0x30);v.y=*(u16 *)(actor+0x32);v.z=*(u16 *)(actor+0x34);
 angle=sub_082215E4((s16)v.x-*(s16 *)(p+0x48),(s16)v.z-*(s16 *)(p+0x4c));angle=(angle+0x20)&0xc0;
 if(p[0xb39]!=angle)sub_081850F4(p);
 else if(sub_0818505C(p,29))*state=1;
 break;}
 case 1:if(p[0xb40]==0xff)*state=0xff;break;
 }
}
