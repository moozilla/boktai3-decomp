#include "global.h"
struct Slot {s16 a,b,c,pad,d,e;u16 flags,value;};
struct Context {u8 pad[0x18];struct Slot slots[8];};
extern struct Context *gUnk_030025FC;
u8 sub_0822B5D0(s32,s32,s32,s32,s32,s32);
s8 sub_0822B7E4(s32 a,s32 b,s32 c,u8 mask,u16 value,s32 index)
{
 struct Context *ctx=gUnk_030025FC;
 if(ctx && index<=7) {
  struct Slot *slot=ctx->slots+index;
  do {
   if(!(slot->flags&4) && (!slot->value || slot->value!=value)) {
    if(slot->flags&0x80) return index;
    if(slot->d>=0 && (slot->flags&mask) && sub_0822B5D0(slot->a,slot->c,slot->d,a,b,c)) return index;
   }
   slot++;index++;
  } while(index<=7);
 }
 return -1;
}
