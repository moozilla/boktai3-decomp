#include "global.h"
struct Slot { u8 bytes[0x38]; };
struct S { u8 p0[0x18]; u16 x,y,z,p1E; struct Slot slots[4]; void *res; };
extern struct S *gUnk_02000098;
void *sub_08215184(u32);
void sub_082329D4(void *,void *);
u32 sub_08232ED4(struct S *s)
{
 struct Slot *p;
 int i;
 gUnk_02000098=s;
 *(void **)((u8*)s+0x100)=sub_08215184(0x1c1a);
 s->x=0x78;
 s->y=0x50;
 s->z=0;
 p=s->slots;
 i=3;
 do { sub_082329D4(s,p); p++; i--; } while(i>=0);
 return 0;
}
