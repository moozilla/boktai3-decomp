// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct CloudPair {
    u32 x, z;
};
struct CloudSource {
    u8 pad0[0x20];
    u32 flags;
    u8 pad24[0xC];
    struct CloudPair position;
};
struct CloudParticle {
    u8 kind;
    u8 frame;
    u8 timer;
    u8 pad3[0x1D];
    u8 sprite[0x28];
};
struct CloudTask {
    u8 pad0[0x18];
    u32 ticks;
    u8 pad1C;
    u8 pad1D;
    u8 pad1E;
    u8 stopFlag;
    u32 frameBase;
    u32 pad24[2];
    u32 sheet;
    u8 pad30[0x48 * 40];
};
extern u8 *gGameState;
extern const u16 gUnk_086126C8[];
extern const u32 gUnk_086126BC[];
void SpriteDrawable_SelectTileFrame(void *, u32, u32);
void sub_08249240(void *, void *, u32);
void Task_Kill(void *);
u32 sub_081D31EC(u8 *p)
{
    struct CloudParticle *e;
    s32 i;
    switch (*(u16 *)(p + 0x1c)) {
        case 1: {
            struct CloudSource *src = *(struct CloudSource **)(p + 0xb70);
            *(struct CloudPair *)(p + 0x24) = src->position;
            *(u16 *)(p + 0x26) += 0x80;
        } break;
        case 2: {
            struct CloudSource *src = *(struct CloudSource **)(p + 0xb70);
            u32 bit = 0x80;
            if (src->flags & bit) {
                *(struct CloudPair *)(p + 0x24) = src->position;
                if (*(s16 *)(gGameState + 0x85e) == 0) {
                    *(u16 *)(p + 0x24) += 0x14;
                    *(u16 *)(p + 0x26) += 0x1e0;
                    *(u16 *)(p + 0x28) -= 0x14;
                } else {
                    *(u16 *)(p + 0x24) -= 0x14;
                    *(u16 *)(p + 0x26) += 0x1e0;
                    *(u16 *)(p + 0x28) += 0x14;
                }
                p[0x1e] = 6;
            } else {
                *(struct CloudPair *)(p + 0x24) = src->position;
                *(u16 *)(p + 0x26) += 0x80;
                p[0x1e] = 4;
            }
        } break;
        case 3: {
            struct CloudSource *src = *(struct CloudSource **)(p + 0xb70);
            *(struct CloudPair *)(p + 0x24) = src->position;
            if (*(s16 *)(gGameState + 0x85e) == 0) {
                *(u16 *)(p + 0x24) += 0x14;
                *(u16 *)(p + 0x26) += 0x1e0;
                *(u16 *)(p + 0x28) -= 0x14;
            } else {
                *(u16 *)(p + 0x24) -= 0x14;
                *(u16 *)(p + 0x26) += 0x1e0;
                *(u16 *)(p + 0x28) += 0x14;
            }
            p[0x1e] = 4;
        } break;
    }
    e = (struct CloudParticle *)(p + 0x30);
    for (i = 39; i >= 0; i--, e = (struct CloudParticle *)((u8 *)e + 0x48)) {
        if (e->kind) {
            register u32 value asm("r0") = e->timer + 1;
            u32 zero;
            register u32 mask asm("r3");
            zero = 0;
            e->timer = value;
            mask = 0xff;
            if ((u8)value >= gUnk_086126C8[e->frame]) {
                e->timer = zero;
                value = e->frame + 1;
                e->frame = value;
                if ((value & mask) > 1)
                    e->frame = zero;
                SpriteDrawable_SelectTileFrame(e->sprite, *(u32 *)(p + 0x2c),
                                               *(u32 *)(p + 0x20) + e->frame);
            }
            sub_08249240(p, e, gUnk_086126BC[e->kind]);
        }
    }
    if (p[0x1f])
        Task_Kill(p);
    *(u32 *)(p + 0x18) += 1;
    return 0;
}
