#include "player_cue.h"
#include "contact_position.h"

void Player_SetAnimation(struct Player *, s32, s32, s32);
s32 ActorVariant_GetTableValue(s32, s32);
void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_Motion7Turn(struct Player *, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(void);
void Player_UpdateTimedCueAnimation(void);
void PlayerCue_UpdateRunTo(struct Player *);

void PlayerCue_RunTo(struct Player *p, const struct ContactPosition *q, u32 b, u32 w)
{
    PlayerCue_Coordinates(p)->destination = *q;
    p->cueParameter = w;
    p->targetFacing = b;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 2, 0);
    p->triggerAnimationCallback = PlayerCue_UpdateRunTo;
}
