#include "global.h"
#define CombatActor_UpdateDefeatEffect sub_08209A38
#define CombatActor_RecordDefeat sub_0820901C
#define gRandomTableIndex gUnk_03005308
#define gRandomTable gUnk_0203B400

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
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void sub_0820901C(void *);
u32 sub_08209A38(struct CombatActor *p)
{
    if (p->defeatEffectDisabled == 0) {
        if (p->timer - 13 <= 10) {
            struct CombatActorEffectPosition v;
            u16 *q = (u16 *)&v;
            u32 idx, t;
            v = p->position;
            q[1] += 0x80;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            idx = gUnk_03005308;
            t = *(u8 *)&gUnk_0203B400[idx];
            SpriteParticle_CreateDefaultVariant4384(&v, 0x80, 0x20, 8, 8, 0x18, t, 0x20, 0x20, 1, 2, 0x200);
        }
        if (p->timer == 1)
            sub_0820901C(p);
    }
    return 0;
}
