#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#define gRandomTableIndex gRandomIndex

/* Tentative family name; the defeat callback records script and combat statistics. */
struct CombatActorEffectPosition {
    u16 x, y;
    u32 z;
};
struct CombatActor {
    u32 timer;
    u8 pad04[0x2c - 4];
    struct CombatActorEffectPosition position;
    u8 pad34[0x136 - 0x34];
    s16 defeatEffectDisabled;
};
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);
void CombatActor_RecordDefeat(void *);
u32 CombatActor_UpdateDefeatEffect(struct CombatActor *p)
{
    if (p->defeatEffectDisabled == 0) {
        if (p->timer - 13 <= 10) {
            struct CombatActorEffectPosition v;
            u16 *q = (u16 *)&v;
            u32 idx, t;
            v = p->position;
            q[1] += 0x80;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            idx = gRandomIndex;
            t = *(u8 *)&gRandomTable[idx];
            SpriteParticle_CreateDefaultVariant4384(&v, 0x80, 0x20, 8, 8, 0x18, t, 0x20, 0x20, 1, 2,
                                                    0x200);
        }
        if (p->timer == 1)
            CombatActor_RecordDefeat(p);
    }
    return 0;
}
