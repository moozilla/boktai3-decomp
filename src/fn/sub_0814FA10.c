#include "global.h"
#include "player.h"
#ifdef NONMATCHING
// fuzzy: 83.7%
// foundry draft (worker best 83.7%); what differs is mostly register allocation and instruction order
#define Player_TurnTowardTargetFacing sub_0814FA10
#define Player_ResetTriggerAnimation  PlayerCue_EnterStandUpdate
#define Player_DispatchPendingTrigger PlayerCue_CompleteAction
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void sub_0814FA10(struct Player *p)
{
    if (p->tetherFrame > 1) {
        s32 target = p->targetFacing;
        u32 facing = p->facing;
        u32 shifted = facing;
        s32 delta;
        shifted -= 8;
        target -= shifted;
        delta = target & 7;
        if (delta > 4)
            facing += 7;
        else
            facing += 1;
        p->facing = facing & 7;
        {
            u8 *bytes = (u8 *)p;
            u32 current = bytes[0x3A4];
            if (current > 4) {
                p->heading = 8 - current;
                p->side = 1;
            } else {
                p->heading = current;
                p->side = 0;
            }
        }
        p->tetherFrame = 0;
    } else
        p->tetherFrame++;
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
    if (p->facing == p->targetFacing) {
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0814FA10);
#endif
