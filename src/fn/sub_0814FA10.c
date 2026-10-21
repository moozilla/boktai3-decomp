#include "global.h"
#include "player.h"
#define Player_ResetTriggerAnimation  PlayerCue_EnterStandUpdate
#define Player_DispatchPendingTrigger PlayerCue_CompleteAction
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Player_TurnTowardTargetFacing(struct Player *p)
{
    if (p->tetherFrame > 1) {
        s32 target = p->targetFacing;
        u8 *fp = &p->facing;
        u32 facing = *fp;
        u32 shifted = facing;
        s32 delta;
        shifted -= 8;
        target -= shifted;
        delta = target & 7;
        *fp = (facing + (delta > 4 ? 7 : 1)) & 7;
        {
            u32 current = p->facing;
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
