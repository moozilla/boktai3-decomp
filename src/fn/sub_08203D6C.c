#include "global.h"
#include "engine_globals.h"
#include "ram.h"
struct V {
    u16 x;
    u16 y;
    u32 z;
};
struct R {
    u8 a;
    u8 b;
} __attribute__((packed));
void SpriteParticle_CreateDefaultVariant4384(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void EnemyF426_RecordDefeat(void *);
s32 EnemyF426_UpdateDeath(u8 *p)
{
    if (!(*((s16 *)(p + 0x144)))) {
        if ((*((s32 *)(p + 0x18))) <= 11) {
            struct V v;
            struct V *q = &v;
            u32 r;
            *q = *((struct V *)(p + 0x40));
            q->y += 200;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            r = (*(&((struct R *)gRandomTable)[gRandomIndex])).a;
            SpriteParticle_CreateDefaultVariant4384(&v, 128, 32, 12, 12, 24, r, 32, 32, 1, 1, 512);
        }
        if ((*((s32 *)(p + 0x18))) == 12) {
            EnemyF426_RecordDefeat(p);
        }
    }
    return 0;
}
