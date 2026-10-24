#include "global.h"
struct PhaseParent {
    u8 unknown0[0xD80];
    u32 flags;
};
struct PhaseActor {
    u8 unknown0[6];
    u8 angle;
    u8 unknown7[0x112];
    u8 flags119;
    u8 unknown11A[4];
    u8 state, state2;
    u8 unknown120[0x40];
    struct PhaseParent *parent;
    void (*update164)(void);
    u8 unknown168[0x10];
    void (*update178)(void);
    u8 unknown17C[8];
    s32 timer;
    u8 targetAngle, unknown189;
    s8 direction, turnSteps;
};
void IvyChild_UpdateAfterBob(void);
static inline void Finish(struct PhaseActor *p, u8 one)
{
    u8 *flags = &p->flags119;
    void (**update)(void) = &p->update164;
    p->state = one;
    p->state2 = one;
    p->flags119 = *flags | one;
    *update = IvyChild_UpdateAfterBob;
}
void Sword_AccelerateReturnTurn(struct PhaseActor *p)
{
    struct PhaseParent *parent = p->parent;
    s32 i;
    for (i = 0; i < p->turnSteps; i++) {
        p->angle += (u8)p->direction;
        if (p->angle == 128) {
            Finish(p, 1);
            return;
        }
    }
    if (!(parent->flags & 7)) {
        if (++p->turnSteps > 6)
            p->turnSteps = 7;
    }
}
