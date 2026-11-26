#define gEffectRandomIndex gRandomIndex
// Orange spine family and descriptive names are tentative (low confidence).
#include "monsters/cannons/plasma_cannon.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
void TrapSpellParticles_CreateDefaultBurst(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);
void PlasmaCannon_RecordDefeat(void *);
u32 PlasmaCannon_UpdateDefeatEffect(struct OrangeSpineActor *p)
{
    if (p->defeatDelay == 0) {
        if ((u32)(p->age - 13) <= 10) {
            struct ContactPosition position;
            struct ContactPosition *pos = &position;
            u32 random;
            *pos = p->sprite.display.position;
            pos->y += 128;
            gEffectRandomIndex = (gEffectRandomIndex + 1) & 0x3ff;
            random = *(u8 *)&gRandomTable[gEffectRandomIndex];
            TrapSpellParticles_CreateDefaultBurst(&position, 128, 32, 12, 12, 24, random, 32, 32,
                                                    1, 2, 512);
        }
        if (p->age == 1)
            PlasmaCannon_RecordDefeat(p);
    }
    return 0;
}
