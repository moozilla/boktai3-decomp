#include "player.h"

void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_Motion7Turn(struct Player *, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(void);
void Player_UpdateTimedCueAnimation(void);
void Sound_PlaySong(s32);

void PlayerCue_Motion30End(struct Player *p, u32 w)
{
    if (p->kind == 2 && p->tetherMode == 0x1e && p->tetherPhase == 5) {
        p->cueParameter = w;
        p->tetherPhase = 4;
        Player_EnterKindTwo(p);
    }
}
