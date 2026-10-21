#include "global.h"
// MATCHING_CONCESSION: register-pinning

// foundry draft (worker best 86.5%); what differs is mostly register allocation and instruction order
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
        register u32 enabled asm("r0");
        register u32 phase asm("r1");
        register void (*update)(void) asm("r3") = parent->update;
        if (update == Sword_UpdateWaveAttack) {
            enabled = 1;
            phase = 2;
        } else if (update == Sword_UpdateThrustAttack) {
            enabled = 1;
            phase = 3;
        } else if (update == Sword_UpdateSpinningLungeAttack) {
            enabled = 1;
            phase = 4;
        } else
            return;
        p->field11E = enabled;
        p->field11F = phase;
        p->flags119 = enabled | p->flags119;
        p->update164 = update;
    }
}
