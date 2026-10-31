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
void Player_UpdateAlternateFormCue(struct Player *);

s32 PlayerCue_Motion11(struct Player *p, u32 w)
{
    p->cueParameter = w;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0xb, 0);
    p->triggerAnimationCallback = Player_UpdateAlternateFormCue;
    return 1;
}
