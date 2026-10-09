// CFLAGS: -O2 -mthumb-interwork -fprologue-bugfix
#include "global.h"
struct Elem { u32 flags; u8 pad4[11]; u8 attr; u8 pad10[0x18]; u16 age,active; u8 pad2C[8]; u32 data; void (*update)(struct Elem *); };
struct Owner { u8 pad[0x8c]; struct Elem elems[8]; };
u32 sub_08215184(u32);
void sub_08217DE0(struct Elem *,u32,s32);
void sub_08217EC4(struct Elem *,s32,s32);
void sub_08217EEC(struct Elem *,u32,s32);
void sub_08217ECC(struct Elem *,s32);
void sub_08234F88(u8 *p)
{
    u32 resource=sub_08215184(0x1c1e);
    s32 i;
    for(i=0;i<8;i++) {
        struct Elem *e=(struct Elem *)(p+0x8c+i*60);
        *(u32 *)(p+0xc0+i*60)=resource;
        sub_08217DE0(e,resource,17);
        sub_08217EC4(e,-4,-4);
        sub_08217EEC(e,*(u32 *)(p+0xc0+i*60),2);
        sub_08217ECC(e,1);
        e->attr=0;
        e->age=0;
        e->active=0;
    }
}
