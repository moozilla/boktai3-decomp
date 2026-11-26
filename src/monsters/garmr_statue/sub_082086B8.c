#include "global.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
struct V {
    u16 x;
    u16 y;
    u32 z;
};
struct R {
    u8 a;
    u8 b;
} __attribute__((packed));
void TrapSpellParticles_CreateDefaultBurst(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void GarmrStatue_HandleDefeat(void *);
s32 sub_082086B8(u8 *p)
{
    s16 h = *((s16 *)(p + 0x146));
    if (!h) {
        if ((*((s32 *)(p + 0x18))) <= 11) {
            struct V v;
            struct V *q = &v;
            u32 r;
            *q = *((struct V *)(p + 0x40));
            q->y += 128;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            r = (*(&((struct R *)gRandomTable)[gRandomIndex])).a;
            TrapSpellParticles_CreateDefaultBurst(&v, 128, 32, 12, 12, 24, r, 32, 32, 1, h, 512);
        }
        if ((*((s32 *)(p + 0x18))) == 12) {
            GarmrStatue_HandleDefeat(p);
        }
    }
    return 0;
}
