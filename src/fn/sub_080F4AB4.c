#include "global.h"

#ifdef NONMATCHING
// fuzzy: 80.0%
// foundry draft (worker best 80.0%); what differs is mostly register allocation and instruction order
#define gRandomIndex gUnk_03005308
#define gRandomTable gUnk_0203B400
struct PhaseActor {
    u8 unknown0[9];
    s8 phase;
    u8 unknownA[0x10F];
    u8 flags119;
    u8 unknown11A[4];
    u8 field11E, field11F;
    u8 unknown120[0x44];
    void (*update164)(void);
    u8 unknown168[4];
    void (*update16C)(void);
    u8 unknown170[0x1A];
    s8 direction;
    u8 unknown18B[4];
    u8 phaseStep;
};
extern u32 gRandomIndex;
extern const u16 gRandomTable[];
void Ivy_BeginChildPulse(void);
void IvyChild_UpdateAfterBob(void);
void ActorPhase_ApproachNeutral(struct PhaseActor *p)
{
    s32 phase = p->phase;
    if (phase > 64) {
        phase += (s8)p->phaseStep;
        if (phase < 64)
            phase = 64;
    }
    p->phase = phase;
    if (phase == 64) {
        s32 direction;
        u32 random;
        u16 one;
        u32 offset;
        p->update16C = Ivy_BeginChildPulse;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = gRandomIndex * 2;
        random = *(const u16 *)(offset + (u32)gRandomTable);
        one = 1;
        random &= one;
        direction = -1;
        if (random)
            direction = 1;
        p->direction = direction;
        p->field11E = one;
        p->field11F = one;
        p->flags119 = one | p->flags119;
        p->update164 = IvyChild_UpdateAfterBob;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", ActorPhase_ApproachNeutral);
#endif
