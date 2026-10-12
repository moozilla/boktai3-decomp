#include "global.h"
#define Presentation_UpdateDropShakeBlink sub_081E2910
#define Presentation_Update sub_081E2B50
#define Presentation_ClearTilemap sub_081E2D74
// Provisional name: Presentation_Update
// MATCHING_CONCESSION: register-pinning
struct Sequence {u8 p0[0x18];u8 resource[0x20];u8 layer0[8];u32 flags0;u8 p1[0x98-0x44];u8 layer1[8];u32 flags1;u8 p2[0xb8-0xa4];s16 x1,y1;u8 p3[0xf8-0xbc];u8 layer2[8];u32 flags2;u8 p4[0x158-0x104];u8 layer3[8];u32 flags3;u8 p5[0x178-0x164];s16 x3,y3;u8 p6[0x1b8-0x17c];u8 layer4[8];u32 flags4;u8 p7[0x21c-0x1c4];u16 option,state,timer;u8 p9[2];u16 frame,last,frameTimer,duration,result,exitTimer;void*handle;void(*callback)(void*);};
void sub_0821980C(void*,void*,s32,s32);void sub_08220D78(void*,void*,s32,s32,s32);void sub_081E28D4(void*);void Presentation_ClearTilemap(void);
extern u16 gUnk_03005260[];extern u16 gUnk_03004FC0;
void sub_0821A0C0(void*);void sub_0821AD08(void*,void*);void Presentation_UpdateDropShakeBlink(void*);
s32 sub_082279A8(s32,s32,s32,s32,s32,s32,s32);void sub_08030B78(s32);void sub_08030DBC(s32);
struct Message {u32 count:16;u32 unused:16;void *values;};
// Advance a scripted presentation, handle skipping, and notify its caller on exit.
s32 Presentation_Update(struct Sequence*p){s32 value;struct Message message;
 if(p->callback)p->callback(p);
 if(p->exitTimer){
  if(++p->exitTimer>16){
   if(p->handle){message.count=1;message.values=&value;value=p->result;sub_0821AD08(p->handle,&message);}
   sub_0821A0C0(p);
  }
 }else{
  if(p->frame==p->last&&p->frameTimer==p->duration-16){
   sub_082279A8(1,4,4,4,4,0xffff,0);p->result=0;p->exitTimer=1;
  }else if(p->timer>16&&(gUnk_03005260[1]&11)){
   sub_082279A8(1,4,4,4,4,0xffff,0);p->result=1;p->exitTimer=1;
  }else if(p->frameTimer>=p->duration&&p->option){
   p->frame++;
   if(p->option==1){sub_08220D78(p->layer2,p->resource,4,1,0);sub_0821980C(p->layer3,p->resource,1,1);p->flags4|=1;}
   else if(p->option==3){p->callback=Presentation_UpdateDropShakeBlink;p->state=0;}
   p->frameTimer=0;sub_08030B78(1);sub_08030DBC(p->frame);
  }
  p->timer++;p->frameTimer++;
 }
 if(p->result==1){register u16 *dest asm("r2")=&gUnk_03004FC0;register u32 value asm("r1")=0x1084;*dest=value;}
}
