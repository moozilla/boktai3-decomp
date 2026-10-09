#include "global.h"
struct Slot { u8 state, b1; u16 h2; s16 h4; u8 p6[2]; u16 x,y,z,hE; u32 f10; u8 p14[0x28-0x14]; u16 ox,oy,oz; u8 p2E[0x48-0x2E]; };
struct S { u8 p0[8]; s16 *vec; struct Slot slots[4]; };
struct V { u16 x,y,z,pad; };
extern u16 gUnk_03005418[];
typedef void (*Fn)(void *, struct S *, struct Slot *);
extern Fn const gUnk_08E879A8[];
static inline s32 scale(s32 x) { s32 r; x *= 3; if(x >= 0) r=x >> 5; else r=-((-x)>>5); return r; }
void sub_08231EF8(void *a, struct S *s)
{
 struct V v;
 u16 *q;
 s16 *p = s->vec;
 s32 x=p[0], z=p[2], y;
 struct Slot *slot, *callslot;
 int i;
 { u16 *q=(u16 *)&v;
 q[0] = scale(x-z);
 z = scale(x+z);
 y = scale(p[1]);
 q[1] = z-y;
 z += y;
 q[0] = q[0]-gUnk_03005418[0]+0x78;
 q[1] = q[1]-gUnk_03005418[1]+0x50;
 z -= gUnk_03005418[2];
 q[2] = z;
 }
 { Fn const *table=gUnk_08E879A8;
 q=(u16 *)&v;
 slot=s->slots; callslot=slot; i=3;
 do {
  table[slot->state](a,s,callslot);
  if(slot->state) {
   slot->ox=slot->x+q[0];
   slot->oy=q[1]+slot->y;
   slot->oz=slot->z+q[2];
   slot->h2++;
  }
  slot++; callslot++; i--;
 } while(i>=0);
 }
}
