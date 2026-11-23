#include "global.h"
#include "ratatosk.h"
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Ratatosk_UpdateDefeatSequence(struct Ratatosk *, s32);
#define Ratatosk_ProcessDefeatState sub_081F79F8
s32 Ratatosk_ProcessDefeatState(struct Ratatosk *p)
{
    if (p->health <= 0 && p->vulnerable) {
        Ratatosk_SetFlags(p, 2);
        Ratatosk_StartReaction(p, (void (*)(void))Ratatosk_UpdateDefeatSequence, 15);
    }
    p->vulnerable = 0;
    if (Ratatosk_TestFlags(p, 2)) {
        Ratatosk_StartAction(p, 1, 0, 1);
    } else if (p->flags & 1) {
        Ratatosk_StartAction(p, 1, 0, 0);
    } else {
        return 0;
    }
    return 1;
}
