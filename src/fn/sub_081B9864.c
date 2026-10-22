#include "global.h"
struct Obj {
    u8 pad[8];
    u32 flags;
    u8 pad2[0x60 - 12];
};
struct Owner {
    u8 pad[0x28];
    u32 flags;
    u8 pad2[0x88 - 0x2c];
    u32 flags88;
    u8 pad3[0xe0 - 0x8c];
    struct Obj objs[10];
};
static inline void Obj_ClearFlag(struct Obj *o)
{
    o->flags &= ~1;
}
void sub_081B9864(struct Owner *p)
{
    p->flags88 &= ~1;
    Obj_ClearFlag(&p->objs[0]);
    Obj_ClearFlag(&p->objs[1]);
    Obj_ClearFlag(&p->objs[2]);
    Obj_ClearFlag(&p->objs[3]);
    Obj_ClearFlag(&p->objs[6]);
    p->flags &= ~1;
    Obj_ClearFlag(&p->objs[8]);
    Obj_ClearFlag(&p->objs[7]);
    Obj_ClearFlag(&p->objs[9]);
}
