#include "monsters/axe_armor/axe_armor.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
struct R {
    u8 a;
    u8 b;
} __attribute__((packed));
void SpriteParticle_CreateDefaultVariant4384(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                             s32, s32);
void AxeArmor_RecordDefeat(void *);
s32 AxeArmor_UpdateDeath(struct AxeArmor *p)
{
    if (!p->initialDead) {
        if (p->timer <= 11) {
            struct ContactPosition v;
            struct ContactPosition *q = &v;
            u32 r;
            *q = p->sprite.position;
            q->y += 200;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            r = (*(&((struct R *)gRandomTable)[gRandomIndex])).a;
            SpriteParticle_CreateDefaultVariant4384(&v, 128, 32, 12, 12, 24, r, 32, 32, 1, 1, 512);
        }
        if (p->timer == 12) {
            AxeArmor_RecordDefeat(p);
        }
    }
    return 0;
}
