#include "player.h"

void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(struct Player *);
void Player_UpdateTimedCueAnimation(void);
void Sound_PlaySong(s32);

void PlayerCue_Motion7(struct Player *p, u32 b, u32 w)
{
    p->animationBranchRequested = b;
    p->cueParameter = w;
    p->facing = 0;
    p->heading = 0;
    p->side = 0;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 7, 0);
    p->triggerAnimationCallback = Player_UpdateSongCueAnimation;
}
