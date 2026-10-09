#include "global.h"
struct Packed {u32 a:16,b:16,c:16,pad:16;};
struct Slot {u16 a,b,c,pad;s16 d,e;u16 flags,value;};
struct Context {u8 pad[0x18];struct Slot slots[8];};
extern struct Context *gUnk_030025FC;
u8 sub_0822B5FC(struct Packed*);
void sub_0822B6C8(u16 a,u16 b,u16 c,u16 d,u8 flags,u16 value)
{
 struct Context *ctx=gUnk_030025FC;
 if(ctx) {
  struct Packed point;
  point.a=a;point.b=0;point.c=b;
  if(!sub_0822B5FC(&point)) {
   int i=0;
   u32 zero=0;
   struct Slot *slot;
   flags|=4;slot=ctx->slots;
   do {
    if(slot->d<0) {
     slot->a=a;slot->b=zero;slot->c=b;slot->value=value;
     slot->d=c;slot->e=d;slot->flags=flags;break;
    }
    slot++;i++;
   } while(i<=7);
  }
 }
}
