#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/engine.h"
#include "engine/input.h"
#include "engine/ram.h"
#include "link/wireless/random.h"
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
        q->destination = *(Mod(RANDOM_NEXT(), 9) + q->points);
        Ratatosk_EnterPhase(p, 2);
    }
    if (p->actionFinished == 1 && q->actionState != 3)
        p->actionAnimation = 1;
}
