#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 94.3%
// foundry draft (worker best 94.3%); what differs is mostly register allocation and instruction order
#define Player_UpdateTimedDirectionAction sub_08150B34
void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void sub_08150B34(struct Player *p)
{
    u8 phase = p->tetherPhase;
    switch (phase) {
        case 0:
            if (!p->tetherFrame) {
                s8 direction = p->facing;
                if ((u8)direction > 4) {
                    p->heading = (8 - p->facing) >> 1;
                    p->side = 1;
                } else {
                    p->heading = (u8)direction >> 1;
                    p->side = phase;
                }
                p->tetherFrame++;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x12), 0x40, 1);
            if (p->terrainRecoveryCounter > 30)
                p->terrainRecoveryCounter -= 30;
            else {
                p->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x1B0);
                ActorSequence_Begin(p, 0x1C, 1);
            }
            break;
        case 1:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x13), 0x40, 1)) {
                u8 direction = p->facing;
                if (direction > 4) {
                    p->heading = 8 - direction;
                    p->side = phase;
                } else {
                    p->heading = direction;
                    p->side = 0;
                }
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08150B34);
#endif
