#include "player.h"

void Player_EnterKindTwo(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_Stand(struct Player *, s32);
void Player_UpdateSongCueAnimation(void);
void Player_UpdateTimedCueAnimation(struct Player *);
void Sound_PlaySong(s32);

void PlayerCue_KneelGasp(struct Player *p, s32 v)
{
    Player_EnterKindTwo(p);
    if (v >= 0)
        p->facing = v;
    Player_UpdateFacingSide(p);
    Sound_PlaySong(0xC6);
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0x15, 2);
    p->triggerAnimationCallback = Player_UpdateTimedCueAnimation;
}
