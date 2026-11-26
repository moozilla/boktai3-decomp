#include "monsters/statue.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
void SpriteParticle_CreateDefaultVariant4384(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void Statue_RecordDefeat(void *);
s32 Statue_UpdateDeath(struct Statue *p)
{
    if (!(p->initialDead)) {
        if ((p->timer) <= 11) {
            struct ContactPosition v;
            struct ContactPosition *q = &v;
            u32 r;
            *q = p->sprite.position;
            q->y += 200;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            r = *(const u8 *)&gRandomTable[gRandomIndex];
            SpriteParticle_CreateDefaultVariant4384(&v, 128, 32, 12, 12, 24, r, 32, 32, 1, 1, 512);
        }
        if ((p->timer) == 12) {
            Statue_RecordDefeat(p);
        }
    }
    return 0;
}
