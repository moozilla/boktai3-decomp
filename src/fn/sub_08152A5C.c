#include "player_cue.h"
// confidence=low: tentative cue role inferred from named callers and callback behavior.
#include "player_cue_sequence.h"
// confidence=low: motion 0x1D applies decaying recoil until standing resumes.

void ActorSequence_Begin(struct Player *, s32, s32);
void Player_AddControlFlags(struct Player *, u32);
void PlayerCue_UpdateHidden(struct Player *);
void PlayerCue_CompleteAction(struct Player *);
void PlayerCue_EnterStandUpdate(struct Player *);
void Player_RestartAnimationFromSetup(struct Player *);
extern const s16 gUnk_086149C4[];
void Sound_PlaySong(u32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void PlayerCue_UpdateRecoilSlide(struct Player *p)
{
    s32 angle;
    s32 value;
    if (!p->tetherFrame) {
        Sound_PlaySong(0xE6);
        p->tetherFrame++;
    }
    Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->setupResource, 1,
                     p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
    if (p->recoilSpeed) {
        angle = ((p->facing + 5) & 7) << 5;
        value = gUnk_086149C4[(angle + 64) & 255] * p->recoilSpeed;
        p->velocityX = -Truncate(value);
        value = gUnk_086149C4[angle] * p->recoilSpeed;
        p->velocityZ = -Truncate(value);
        p->recoilSpeed = (p->recoilSpeed * 3) >> 2;
    } else {
        p->recoilSpeed = 0;
        ActorSequence_Begin(p, 0, 0);
        p->triggerAnimationCallback = Player_RestartAnimationFromSetup;
        PlayerCue_CompleteAction(p);
    }
}
