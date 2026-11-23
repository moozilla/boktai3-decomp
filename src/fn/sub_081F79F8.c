#include "global.h"
#include "ratatosk.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Ratatosk_UpdateDefeatSequence(struct Ratatosk *, s32);
static inline void SetState(struct Ratatosk *p, void (*update)(void), u8 state)
{
    p->actionState = state;
    p->actionCallback = update;
    p->callbackFrame = 0;
    p->actionChanged = 1;
    p->reactionPhase = 0;
    p->reactionTimer = 0;
}
static inline u32 TestFlag(u32 mask, u32 *flags)
{
    return *flags & mask;
}
#define Ratatosk_ProcessDefeatState sub_081F79F8
s32 Ratatosk_ProcessDefeatState(struct Ratatosk *p)
{
    if (p->health <= 0 && p->vulnerable) {
        Ratatosk_SetFlags(p, 2);
        SetState(p, (void (*)(void))Ratatosk_UpdateDefeatSequence, 15);
    }
    p->vulnerable = 0;
    if (TestFlag(2, &p->flags)) {
        u32 one = 1;
        p->actionMode = one;
        p->actionIndex = 0;
        p->actionPhase = one;
        p->actionTimer = 0;
        p->actionEnter = 1;
    } else if (p->flags & 1) {
        p->actionMode = 1;
        p->actionIndex = 0;
        p->actionPhase = 0;
        p->actionTimer = 0;
        p->actionEnter = 1;
    } else {
        return 0;
    }
    return 1;
}
