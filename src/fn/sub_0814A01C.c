#include "global.h"
#include "player.h"
void Sound_PlaySong(u32);
u32 Player_ConsumeStateEntry(struct Player *);
void Player_AddControlFlags(struct Player *, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_SetDirectionalVelocity(struct Player *, s32);
void Player_UpdateFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void Player_UpdateStatBAction(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->currentStatB) {
            Sound_PlaySong(0x392);
            p->currentStatB--;
        } else {
            Sound_PlaySong(0x192);
            Player_SetActionPhase(p, 1);
        }
    }
    switch (p->tetherPhase) {
        case 0:
            if (++p->tetherFrame > 15) {
                ActorSequence_Begin(p, 0, 0);
            } else {
                if (p->tetherFrame <= 7) {
                    p->actionEighteenParameter = 0x172;
                    p->actionEighteenDuration = 0x20;
                }
                if (!p->movementVelocityLocked)
                    Player_SetDirectionalVelocity(p, 0x20);
                Player_UpdateFacingSide(p);
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 0x40, 1);
            }
            break;
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
            if (++p->tetherFrame > 7)
                ActorSequence_Begin(p, 0, 0);
            break;
    }
}
