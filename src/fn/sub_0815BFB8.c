#include "player.h"

void Player_SetAnimation(struct Player *, s32, s32, s32);
s32 ActorVariant_GetTableValue(s32, s32);
void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void PlayerCue_Motion7Turn(struct Player *, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(void);
void Player_UpdateTimedCueAnimation(void);
void Sound_PlaySong(s32);
void Player_UpdateThreePhaseBaseAnimationAction(struct Player *);
void ActorSequence_Begin();

void PlayerCue_Motion17(struct Player *p)
{
    Player_EnterKindTwo(p);
    p->facing = 1;
    p->heading = 0;
    p->side = 0;
    ActorSequence_Begin(p, 0x11);
    p->triggerAnimationCallback = Player_UpdateThreePhaseBaseAnimationAction;
}
