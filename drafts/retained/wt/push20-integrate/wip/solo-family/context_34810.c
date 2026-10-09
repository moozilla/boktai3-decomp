#include "global.h"
struct Pair47 { u32 a, b; };
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(u8 *), void (*)(u8 *));
void sub_0821A0C0(u8 *);
void sub_0823450C(u8 *);
void sub_08234520(u8 *);
s32 sub_08234770(u8 *, u8 *, struct Pair47 *);
u8 *sub_082347C8(u8 *p, struct Pair47 *q)
{
    u8 *obj = sub_08219FBC(8, 0x208);
    if (obj) {
        sub_0821A04C(obj, sub_0823450C, sub_08234520);
        if (sub_08234770(obj, p, q) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
#include "global.h"
struct Effect { u32 flags; u8 pad4[0x14]; u16 x,y,z; u8 pad1E[10]; u32 age:16,active:16,dx:16,dy:16,dz:16,pad32:16; void *data; void (*update)(struct Effect *); };
void sub_08217EEC(struct Effect *,void *,u32);
static inline u32 frame(u16 age) { return ((age>>2)&1)+2; }
void sub_08234810(struct Effect *p)
{
    u32 age=p->age+1;
    p->age=age;
    if(p->age>15) { p->flags|=1; p->active=0; }
    else {
        u32 dx,x,dy,y;
        sub_08217EEC(p,p->data,frame(p->age));
        dx=p->dx; x=p->x; p->x=dx+x;
        dy=p->dy; y=p->y; p->y=dy+y;
        if(!(p->age&7)) p->dy=dy+1;
    }
}
