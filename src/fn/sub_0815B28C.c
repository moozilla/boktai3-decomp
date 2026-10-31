#include "player.h"

void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(struct Player *);
void Player_UpdateTimedCueAnimation(void);
void Sound_PlaySong(s32);

void PlayerCue_Motion7End(struct Player *p, u32 w)
{
    if (p->kind == 2 && p->tetherMode == 7 && p->tetherPhase != 0) {
        p->cueParameter = w;
        Player_EnterKindTwo(p);
        ActorSequence_Begin(p, 7, 3);
        p->triggerAnimationCallback = Player_UpdateSongCueAnimation;
    }
}
