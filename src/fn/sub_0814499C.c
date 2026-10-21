#define Player_ConsumePhaseEntry Player_ConsumeStateEntry
#include "global.h"
#include "player.h"
u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetActionPhase(struct Player *, u8);
s32 Player_CanSpendSharedResource(struct Player *, s32);
s32 PlayerBarrier_TryPlace(void *, struct SceneVec *);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
void Player_UpdateActionEffectSequence(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->tetherStatusFlags)
            p->actionEighteenEnabled = 0;
        else if (Player_CanSpendSharedResource(p, 3))
            p->actionEighteenEnabled = 1;
        else
            p->actionEighteenEnabled = 0;
        Player_SetActionPhase(p, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimation(p, 0x8B, 64, 1)) {
                if (!PlayerBarrier_TryPlace(p->actionRequestEffect, &p->position))
                    Sound_PlaySong(0x192);
                else {
                    Sound_PlaySong(0xD0);
                    Sound_PlaySong(0x3E6);
                }
                Player_SetActionPhase(p, 1);
            }
            break;
        case 1:
            if (++p->tetherFrame > 11)
                Player_SetActionPhase(p, 3);
            break;
        case 3:
            if (Player_SetAnimation(p, 0x90, 64, 1))
                ActorSequence_Begin(p, 0, 1);
            break;
    }
}
