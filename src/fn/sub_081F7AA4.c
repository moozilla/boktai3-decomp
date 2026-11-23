#include "global.h"
#include "ratatosk.h"
#include "engine.h"
#include "input.h"
#include "ram.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
s32 Mod(s32, s32);
void AnimationActor_ProcessKeys(struct Ratatosk *p)
{
    struct Ratatosk *q = p;
    if (gPadState[1] & 0x100) {
        if (++p->animationIndex > 36)
            p->animationIndex = 0;
        p->actionCounter = 0;
        p->actionAnimation = p->animationIndex;
    }
    if (gPadState[1] & 0x200) {
        q->duration = 128;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        q->destination =
            *(struct ContactPosition
                  *)((Mod(*(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable), 9) * 8 + (u32)q) +
                     0xccc);
        {
            u32 state = 2;
            p->actionPhase = state;
        }
        p->actionTimer = 0;
        p->actionEnter = 1;
    }
    if (p->actionFinished == 1 && q->actionState != 3)
        p->actionAnimation = 1;
}
