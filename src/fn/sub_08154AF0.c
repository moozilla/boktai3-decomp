#include "player_cue.h"
#include "player_cue_sequence.h"
// confidence=low: tentative cue role from callback phases.

void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_CompleteAction(struct Player *);
void Player_RestartAnimationFromResource(struct Player *);
void Player_UpdateFacingSide(struct Player *);

extern const s16 gUnk_086149C4[];
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

// confidence=low: inferred from the side-facing cue callbacks and effects.
void PlayerCue_UpdateSideRecoil(struct Player *p)
{
    if (!p->tetherFrame) {
        Sound_PlaySong(0xE6);
        ++p->tetherFrame;
    }
    Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource, 2,
                     p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
    if (p->recoilSpeed) {
        u8 *direction;
        u32 angle;
        direction = &p->facing;
        angle = ((*direction + 5) & 7) << 5;
        p->velocityX = -Truncate(gUnk_086149C4[(angle + 64) & 255] * p->recoilSpeed);
        p->velocityZ = -Truncate(gUnk_086149C4[angle] * p->recoilSpeed);
        p->recoilSpeed = (p->recoilSpeed * 3) >> 2;
    } else {
        p->recoilSpeed = 0;
        Player_UpdateFacingSide(p);
        ActorSequence_Begin(p, 0, 0);
        p->triggerAnimationCallback = Player_RestartAnimationFromResource;
        PlayerCue_CompleteAction(p);
    }
}
