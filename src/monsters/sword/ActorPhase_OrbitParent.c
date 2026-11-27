#include "global.h"

struct PhaseParent {
    u8 unknown0[0xB84];
    s16 x;
    u8 unknownB86[2];
    s16 z;
    u8 unknownB8A[0x15E];
    void (*update)(void);
};
struct PhaseActor {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
    u8 unknown22[0xF7];
    u8 flags119;
    u8 unknown11A[4];
    u8 field11E, field11F;
    u8 unknown120[0x40];
    struct PhaseParent *parent;
    void (*update164)(void);
    u8 unknown168[0x1C];
    s32 timer;
    u8 unknown188;
    u8 angle, angleStep;
};
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
void Sword_UpdateWaveAttack(void);
void Sword_UpdateThrustAttack(void);
void Sword_UpdateSpinningLungeAttack(void);
static inline void SetMode(struct PhaseActor *p, u8 enabled, u8 phase)
{
    p->field11E = enabled;
    p->field11F = phase;
    p->flags119 |= enabled;
}
void ActorPhase_OrbitParent(struct PhaseActor *p)
{
    struct PhaseParent *parent = p->parent;
    p->angle += p->angleStep;
    if (p->timer > 0) {
        p->x +=
            Div(parent->x + Div(gUnk_086149C4[(p->angle + 64) & 255] * 200, 4096) - p->x, p->timer);
        p->z += Div(parent->z + Div(gUnk_086149C4[p->angle] * 200, 4096) - p->z, p->timer);
    }
    if (--p->timer == 0) {
        void (*update)(void) = parent->update;
        if (update == Sword_UpdateWaveAttack) {
            SetMode(p, 1, 2);
            p->update164 = update;
        } else if (update == Sword_UpdateThrustAttack) {
            SetMode(p, 1, 3);
            p->update164 = update;
        } else if (update == Sword_UpdateSpinningLungeAttack) {
            SetMode(p, 1, 4);
            p->update164 = update;
        }
    }
}
