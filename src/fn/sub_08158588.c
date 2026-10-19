#include "global.h"
#include "player.h"
struct TriggerOpener;
s32 Trigger_TryOpenNearby(struct TriggerOpener *);
void ActorSequence_Begin(struct Player *, s32, s32);
s32 Player_SelectAlternateInteractionMode(struct Player *p)
{
    s32 mode;
    if (p->tetherMode != 0x17 && (s8)p->movementAnimationState >= 0) {
        p->interactionResetCounter = 0;
        p->directionFrame = 0;
        p->terrainRecoveryCounter = 0;
        ActorSequence_Begin(p, 0x17, 0);
    }
    mode = p->tetherMode;
    if (p->directionFrame != 0)
        mode = 0x1D;
    else if (mode != 0x1D && mode != 0x17 && mode != 0x16 && mode != 0x1C && mode != 4 &&
             mode != 0xE) {
        if (p->inputKeys[1] & 1) {
            if ((u8)Trigger_TryOpenNearby((struct TriggerOpener *)((u8 *)p + 0x24)))
                mode = 0;
            else
                mode = 4;
        } else if ((s16)p->directionHistory[0] == -1 || (p->inputKeys[0] & 0x100))
            mode = 0;
        else
            mode = 1;
    }
    return mode;
}
