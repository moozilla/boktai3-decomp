#include "global.h"
#include "player.h"
#ifdef NONMATCHING
// fuzzy: 98.7%
// foundry draft (worker best 98.7%); what differs is mostly register allocation and instruction order
#define Player_UpdateTimedCueAnimation sub_081507F4
#define Player_SetAnimationPhase ActorSequence_Begin
#define Player_SetAnimationFlags Player_AddControlFlags
#define Player_ResetTriggerAnimation PlayerCue_EnterStandUpdate
#define Player_DispatchPendingTrigger PlayerCue_CompleteAction
void Sound_PlaySong(u32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
s32 ActorVariant_GetTableValue(u32, s32);
void PlayerCue_EnterStandUpdate(struct Player *);
void PlayerCue_CompleteAction(struct Player *);
void sub_081507F4(struct Player *p)
{
    switch (p->tetherPhase) {
    case 0:
        if (!p->tetherFrame) Sound_PlaySong(0xD0);
        if (p->tetherFrame == 8) Sound_PlaySong(0xDB);
        p->tetherFrame++;
        if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x31), 0x40, 1)) {
            ActorSequence_Begin(p, 21, 1);
            goto dispatch;
        }
        break;
    case 1:
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x32), 0x40, 1);
        break;
    case 3:
        if (!p->tetherFrame) Sound_PlaySong(0xD0);
        if (++p->tetherFrame > 4) goto reset;
    case 2:
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x33), 0x40, 1);
        break;
reset:
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
        PlayerCue_EnterStandUpdate(p);
dispatch:
        PlayerCue_CompleteAction(p);
        break;
    case 4:
        {
            u16 frame = p->tetherFrame;
            s32 mask = 7;
            s32 low = mask;
            low &= frame;
            frame >>= 3;
            mask -= frame;
            if (low >= mask) Player_AddControlFlags(p, 5);
        }
        if (++p->tetherFrame > 55) ActorSequence_Begin(p, 21, 5);
        break;
    case 5:
        Player_AddControlFlags(p, 5);
        break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081507F4);
#endif
