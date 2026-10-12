#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Sequence {u8 p0[0x18];u8 resource[0x20];u8 layer0[8];u32 flags0;u8 p1[0x98-0x44];u8 layer1[8];u32 flags1;u8 p2[0xb8-0xa4];s16 x1,y1;u8 p3[0xf8-0xbc];u8 layer2[8];u32 flags2;u8 p4[0x158-0x104];u8 layer3[8];u32 flags3;u8 p5[0x178-0x164];s16 x3,y3;u8 p6[0x1b8-0x17c];u8 layer4[8];u32 flags4;u8 p7[0x21c-0x1c4];u16 option,state,timer;u8 p9[2];u16 frame,last,frameTimer,duration,result,exitTimer;void*handle;void(*callback)(void*);};
void sub_0821980C(void*,void*,s32,s32);void sub_08220D78(void*,void*,s32,s32,s32);void sub_081E28D4(void*);void sub_081E2D74(void);
extern u16 gUnk_0203B400[];extern u32 gUnk_03005308;
s32 Mod(s32,s32);s32 Div(s32,s32);
static inline s32 random(u16 *input){register u16*table asm("r6")=input;gUnk_03005308=(gUnk_03005308+1)&1023;return table[gUnk_03005308];}
// Animate a presentation layer dropping, shaking, then blinking with its alternate.
void sub_081E2910(struct Sequence*p){
 if(p->state<30){p->flags1&=~1;p->x1=60;p->y1=-30;p->flags3|=1;}
 else if(p->state<45){p->flags1&=~1;p->x1=60;{s32 y=p->state*2-90;register s16*dest asm("r3")=&p->y1;*dest=y;}p->flags3|=1;}
 else if(p->state<50){p->x1=Mod(random(gUnk_0203B400),5)+58;{s32 y=(45-p->state)*2+Mod(random(gUnk_0203B400),5)-2;register s16*dest asm("r3")=&p->y1;*dest=y;}}
 else if(p->state<55){p->x1=Mod(random(gUnk_0203B400),5)+58;{s32 y=(p->state-55)*2+Mod(random(gUnk_0203B400),5)-2;register s16*dest asm("r3")=&p->y1;*dest=y;}}
 else if(p->state<60){p->x1=Mod(random(gUnk_0203B400),5)+58;p->y1=Mod(random(gUnk_0203B400),5)-2;p->flags3|=1;}
 else if(p->state<80){p->x1=Mod(random(gUnk_0203B400),3)+59;p->y1=Mod(random(gUnk_0203B400),3)-1;p->flags3|=1;}
 else if(p->state<90){p->x1=60;p->y1=0;}
 else if(Div(p->state,45)&1){p->flags1|=1;p->flags3&=~1;}
 else {p->flags1&=~1;p->flags3|=1;}
 p->state++;
}
