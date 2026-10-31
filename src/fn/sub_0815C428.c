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
void PlayerCue_UpdateFormChange(struct Player *);
void Player_UpdateAlternateFormCue(struct Player *);

s32 PlayerCue_FormMotion(struct Player *p, u32 w)
{
    s32 r;
    p->cueParameter = w;
    if (p->actionMode == 0) {
        r = 0;
    } else {
        Player_EnterKindTwo(p);
        if (p->actionMode == 1) {
            ActorSequence_Begin(p, 0xa, 0);
            p->triggerAnimationCallback = PlayerCue_UpdateFormChange;
        } else if (p->actionMode == 2) {
            ActorSequence_Begin(p, 0xb, 0);
            p->triggerAnimationCallback = Player_UpdateAlternateFormCue;
        }
        r = 1;
    }
    return r;
}
