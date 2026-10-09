#include "global.h"
struct Elem { u32 flags; u8 pad4[11]; u8 attr; u8 pad10[0x18]; u16 age,active; u8 pad2C[8]; u32 data; void (*update)(struct Elem *); };
struct Owner { u8 pad[0x168]; struct Elem elems[8]; };
u32 sub_08215184(u32);
void sub_08217DE0(struct Elem *,u32,s32);
void sub_08217EC4(struct Elem *,s32,s32);
void sub_08217EEC(struct Elem *,u32,s32);
void sub_08217ECC(struct Elem *,s32);
void sub_08235AB8(struct Owner *p)
{
    u32 resource=sub_08215184(0x1c1e);
    u32 zero=0;
    struct Elem *e=(struct Elem *)((u8 *)p+0x168);
    s32 i;
    for(i=0;i<8;e++,i++) {
        p->elems[i].data=resource;
        sub_08217DE0(e,resource,17);
        sub_08217EC4(e,-4,-4);
        sub_08217EEC(e,p->elems[i].data,2);
        sub_08217ECC(e,1);
        e->attr=zero;
        e->age=zero;
        e->active=zero;
    }
}
