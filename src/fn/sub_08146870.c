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
void Player_UpdateBriefAction(struct Player *p)
{
    if (Player_ConsumeStateEntry(p))
        Sound_PlaySong(0x57D);
    Player_AddControlFlags(p, 8);
    if (++p->tetherFrame > 15) {
        ActorSequence_Begin(p, 0, 0);
    } else {
        Player_AddControlFlags(p, 2);
        if ((u16)(p->tetherFrame - 5) <= 7)
            Player_AddControlFlags(p, 1);
        Player_AddControlFlags(p, 4);
        p->actionEighteenParameter = 0x174;
        p->actionEighteenDuration = 0x18;
        if (!p->movementVelocityLocked)
            Player_SetDirectionalVelocity(p, 0x20);
        Player_UpdateFacingSide(p);
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 0x40, 1);
    }
}
