#include "monsters/statue.h"
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
void SpriteParticle_CreateDefaultVariant4384(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void Statue_RecordDefeat(void *);
s32 Statue_UpdateDeath(struct Statue *p)
{
    if (!(p->initialDead)) {
        if ((p->timer) <= 11) {
            struct V v;
            struct V *q = &v;
            u32 r;
            *q = *(struct V *)&p->sprite.position;
            q->y += 200;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            r = (*(&((struct R *)gRandomTable)[gRandomIndex])).a;
            SpriteParticle_CreateDefaultVariant4384(&v, 128, 32, 12, 12, 24, r, 32, 32, 1, 1, 512);
        }
        if ((p->timer) == 12) {
            Statue_RecordDefeat(p);
        }
    }
    return 0;
}
