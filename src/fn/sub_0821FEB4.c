#include "global.h"
struct V {
    s16 x, y, z, pad;
};
struct Collider {
    u8 pad0[4];
    u16 id;
    u16 f6;
    u16 f8;
    u16 fa;
    u8 padc[8];
    struct V offset;
    struct V size;
    u8 pad24[0x34 - 0x24];
    u32 f34;
    u32 f38;
    u8 pad3c[2];
    u16 f3e;
    u16 f40;
    u8 pad42;
    u8 f43;
    u8 pad44[3];
    u8 f47, f48;
    u8 pad49[3];
    u32 f4c;
};
void Collider_Init(struct Collider *c, u16 id, u32 flags, u32 a, u32 b, struct V *size,
                   struct V *offset)
{
    c->f6 = flags;
    c->id = id;
    c->f8 = a;
    c->fa = b;
    if (size != 0)
        *(u64 *)&c->size = *(u64 *)size;
    else {
        c->size.x = 0;
        c->size.y = 0;
        c->size.z = 0;
    }
    if (offset != 0)
        *(u64 *)&c->offset = *(u64 *)offset;
    else {
        c->offset.x = 0;
        c->offset.y = 0;
        c->offset.z = 0;
    }
    c->f43 = 0;
    c->f3e = 0;
    c->f34 = 0;
    c->f38 = 0;
    c->f40 = 0;
    c->f4c = 0;
    c->f47 = 1;
    c->f48 = 0x40;
}
