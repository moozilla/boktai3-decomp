#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {s16 x,y,z,pad;};
struct Actor {u8 pad[48];struct Vec position;};
struct Manager {u8 task[24];void *graphics,*resource;u16 kind;u8 pad22[6];void *head;struct Actor *actors[4];struct Vec projected[4];};
extern struct Manager *gUnk_02000030;
extern struct Actor *gUnk_02000580[];
void *sub_08215184(s32);void *sub_0821A520(s32,s32);
static inline s32 scale(s32 input){register s32 value asm("r0")=input*3;asm("" : : "r"(value));if(value>=0)value>>=5;else value=-((-value)>>5);asm("" : : "r"(value));return value;}
static inline s32 scaleHeight(s32 input){register s32 value asm("r0")=input*3;asm("" : : "r"(value));if(value>=0)return value>>5;return -((-value)>>5);}
#define TriggerManager_Initialize sub_08008174
s32 TriggerManager_Initialize(struct Manager *p){
 s32 i;gUnk_02000030=p;p->graphics=sub_08215184(7194);p->kind=14454;p->resource=sub_0821A520(37422,39666);p->head=0;
 for(i=0;i<4;i++){
  struct Actor *actor;asm("" : "+r"(i));actor=({register struct Actor *a asm("r2")=gUnk_02000580[i];asm("" : : "r"(a));a;});p->actors[i]=actor;
  if(actor){register u32 offset asm("r0")=i*8;register struct Vec *out asm("r3");register struct Vec *in asm("r5");s32 x,z,horizontal,height;asm("" : : "r"(offset));out=(struct Vec *)(offset+(u32)p);asm("" : : "r"(out));out=(struct Vec *)((u8 *)out+60);in=&actor->position;x=({register s32 value asm("r4")=actor->position.x;asm("" : : "r"(value));value;});z=in->z;
   out->x=scale(x-z);horizontal=scale(({register s32 sum asm("r1")=x+z;asm("" : : "r"(sum));sum;}));height=scaleHeight(in->y);out->y=horizontal-height;out->z=horizontal+height;
  }
 }
 return 0;
}
