#include "global.h"
#include "player.h"
s32 Player_CanSpendSharedResource(struct Player *, s32);
void *SolarBomb_Create(struct Player *, struct SceneVec *, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Sound_PlaySong(u32);
void Player_UpdateActionEighteen(struct Player *p)
{
    if (p->tetherMode != 18) {
        if (p->tetherStatusFlags)
            p->actionEighteenEnabled = 0;
        else if (Player_CanSpendSharedResource(p, 3)) {
            p->actionEighteenEnabled = 1;
            SolarBomb_Create(p, &p->position, 0);
        } else
            p->actionEighteenEnabled = 0;
        ActorSequence_Begin(p, 18, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 32;
            }
            if (Player_SetAnimation(p, 0x8B, 64, 1)) {
                if (p->actionEighteenEnabled)
                    ActorSequence_Begin(p, 18, 1);
                else
                    ActorSequence_Begin(p, 18, 2);
            }
            break;
        case 1:
            if (p->actionEighteenEnabled) {
                p->actionEighteenParameter = 0x170;
                p->actionEighteenDuration = 32;
            }
            if (++p->tetherFrame > 11)
                ActorSequence_Begin(p, 18, 3);
            break;
        case 2:
            if (p->tetherFrame == 0)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, 0x95, 64, 1);
            if (++p->tetherFrame > 11)
                ActorSequence_Begin(p, 18, 3);
            break;
        case 3:
            if (Player_SetAnimation(p, 0x90, 64, 1))
                ActorSequence_Begin(p, 0, 1);
            break;
    }
}
