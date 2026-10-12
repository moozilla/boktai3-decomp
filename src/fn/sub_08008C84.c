#include "global.h"
struct Slot {u8 pad[5];u8 flash;u8 pad6[2];s16 timeout;u8 padA[2];s32 counter;u8 pad10[8];u32 kind;u8 pad1C[24];u8 drawable[180];void *resource;};
extern u16 gUnk_086049CE[][2];extern u16 gUnk_086049DA[];
s32 sub_08008AA4(struct Slot *);void sub_08008A98(struct Slot *,s32);void sub_08220D78(void *,void *,u32,u32,u32);void sub_08219A80(void *,u32);
#define Breakable_Idle sub_08008C84
void Breakable_Idle(void *manager,struct Slot *slot){
 if(sub_08008AA4(slot)){
  void *drawable=slot->drawable;
  sub_08220D78(drawable,slot->resource,gUnk_086049CE[slot->kind][1],1,0);
  sub_08219A80(drawable,gUnk_086049DA[slot->kind]);
 }
 if(slot->timeout>=0&&slot->counter>=slot->timeout)sub_08008A98(slot,4);
}
