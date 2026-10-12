#include "global.h"
struct Slot {u8 pad[5];u8 flash;u8 pad6[2];s16 timeout;u8 padA[2];s32 counter;u8 pad10[8];u32 kind;u8 pad1C[24];u8 drawable[32];struct {u32 x:16,y:16,z:16,pad:16;} position;u8 rest[140];void *resource;};
extern u16 gUnk_086049CE[][2];extern u16 gUnk_086049DA[];
s32 sub_08008AA4(struct Slot *);void sub_08008A98(struct Slot *,s32);void sub_08220D78(void *,void *,u32,u32,u32);void sub_08219A80(void *,u32);

struct Vec {u32 x:16,y:16,z:16,pad:16;};
void sub_08008BD0(struct Slot *);
void sub_08015BD4(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,struct Vec *);
#define Breakable_Activate sub_08008D64
void Breakable_Activate(void *manager,struct Slot *slot){
 struct Vec effect;
 if(sub_08008AA4(slot)){
  void *drawable=slot->drawable;
  sub_08220D78(drawable,slot->resource,gUnk_086049CE[slot->kind][0],1,0);
  slot->flash=10;sub_08008BD0(slot);
  if(slot->kind!=2){
   sub_08219A80(drawable,gUnk_086049DA[slot->kind]+1);
   effect=*(struct Vec *)&slot->position;
   effect.x=slot->position.x;effect.y=slot->position.y+400;effect.z=slot->position.z;
   sub_08015BD4(1,2,0x2001,64,0,0,0,0,4,2,&effect);
   effect.x=slot->position.x-64;effect.y=slot->position.y+192;effect.z=slot->position.z+64;
   sub_08015BD4(1,2,0x2001,64,0,0,0,0,4,2,&effect);
   effect.x=slot->position.x+64;effect.y=slot->position.y+192;effect.z=slot->position.z-64;
   sub_08015BD4(1,2,0x2001,64,0,0,0,0,4,2,&effect);
  }else{
   effect=*(struct Vec *)&slot->position;
   effect.x=slot->position.x;effect.y=slot->position.y+200;effect.z=slot->position.z;
   sub_08015BD4(1,2,0x2001,64,0,0,0,0,4,2,&effect);
  }
 }
 if(slot->counter>10)sub_08008A98(slot,1);
}
