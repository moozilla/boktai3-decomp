#include "global.h"
struct Part {
    u8 pad[8];
    u32 flags;
    u8 pad2[0x54];
};
struct Obj {
    u8 pad[0x78];
    struct Part parts[6];
};

static inline void Part_Enable(struct Part *part)
{
    part->flags |= 1;
}

void sub_081C5660(struct Obj *p)
{
    s32 i;
    for (i = 0; i < 4; i++)
        Part_Enable(&p->parts[i]);
    Part_Enable(&p->parts[4]);
    Part_Enable(&p->parts[5]);
}
