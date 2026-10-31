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
void PlayerCue_UpdateRunOverlay(struct Player *);

void PlayerCue_Run(struct Player *p, s32 v)
{
    if (v >= 0)
        p->facing = v;
    Player_UpdateFacingSide(p);
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 0x20, 1);
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 2, 0);
    p->triggerAnimationCallback = PlayerCue_UpdateRunOverlay;
}
