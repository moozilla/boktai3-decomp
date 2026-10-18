#include "global.h"

struct ArmorParent {
    u8 pad0[0x5C4];
    u8 activePartCount;
};
struct ArmorPart {
    u32 flags;
    u8 pad4[2];
    u8 rotation;
    u8 pad7[0x17];
    s16 y;
    u8 pad20[0xD2];
    u16 velocity;
    u8 padF4[0x25];
    u8 flags119;
    u8 pad11A[4];
    u8 byte11E;
    u8 state;
    u8 pad120[0x3C];
    struct ArmorParent *parent;
    u8 pad160[0x70];
    void (*callback)(struct ArmorPart *);
    u8 pad1D4[2];
    s16 timer;
};
void Armor_UpdatePartRebound(struct ArmorPart *p)
{
    p->y += p->velocity;
    p->velocity -= 2;
    p->rotation += 20;
    if (++p->timer > 30) {
        struct ArmorParent *parent = p->parent;
        u32 state = 5;
        u8 one;
        p->byte11E = 0;
        p->state = state;
        one = 1;
        p->flags119 |= one;
        parent->activePartCount--;
        p->callback = 0;
        p->flags = (p->flags & ~512) | one;
    }
}
