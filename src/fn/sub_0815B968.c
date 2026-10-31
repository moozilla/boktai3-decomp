#include "player.h"

void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_Motion7Turn(struct Player *, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(void);
void Player_UpdateTimedCueAnimation(void);
void Sound_PlaySong(s32);
void PlayerCue_UpdateScaleTransition(struct Player *);

void PlayerCue_Motion7Variant7(struct Player *p, s32 v, u32 w)
{
    if ((u8)(p->actionMode - 3) <= 2)
        PlayerCue_Motion7Turn(p, v);
    if (v >= 0)
        p->facing = v;
    Player_UpdateFacingSide(p);
    p->cueParameter = w;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 7, 7);
    p->triggerAnimationCallback = PlayerCue_UpdateScaleTransition;
}
