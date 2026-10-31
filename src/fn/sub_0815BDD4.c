#include "player.h"

void Player_SetAnimation(struct Player *, s32, s32, s32);
s32 ActorVariant_GetTableValue(s32, s32);
void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_Motion7Turn(struct Player *, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(void);
void Player_UpdateTimedCueAnimation(void);
void Sound_PlaySong(s32);
void PlayerCue_UpdateAim(struct Player *);

void PlayerCue_AimHold(struct Player *p, s32 v, u32 b, u32 w)
{
    if (v >= 0)
        p->facing = v;
    Player_UpdateFacingSide(p);
    p->cueParameter = w;
    p->cuePhase = b;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 3, 0);
    p->triggerAnimationCallback = PlayerCue_UpdateAim;
}
