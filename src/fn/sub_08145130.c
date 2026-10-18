#include "global.h"
#include "player.h"
void Player_AddControlFlags(struct Player *, s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Sound_PlaySong(u32);
void Player_UpdateCountdownAnimation(struct Player *p)
{
    Player_AddControlFlags(p, 8);
    switch (p->tetherPhase) {
        case 0:
            if (p->animationIndex != 0x2C) {
                p->facing |= 1;
                if (p->facing > 4) {
                    p->heading = (8 - p->facing) >> 1;
                    p->side = 1;
                } else {
                    p->heading = p->facing >> 1;
                    p->side = 0;
                }
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 18), 64, 1);
            if (p->actionCountdown <= 30) {
                p->actionCountdown = 0;
                Sound_PlaySong(0x1B0);
                Player_SetActionPhase(p, 1);
            } else
                p->actionCountdown -= 30;
            break;
        case 1:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 19), 64, 1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
