#include "player_cue.h"
// confidence=low: tentative cue role inferred from named callers and callback behavior.
#include "player_cue_sequence.h"

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
void PlayerCue_UpdateWalkTarget(struct Player *p)
{
    s32 dx = PlayerCue_Motion(p)->destination.x - p->position.x;
    s32 dy = PlayerCue_Motion(p)->destination.z - p->position.z;
    s32 angle = Math_Atan2(dx, dy);
    if (dx * dx + dy * dy <= 81) {
        p->position.x = PlayerCue_Motion(p)->destination.x;
        p->position.z = PlayerCue_Motion(p)->destination.z;
        if (p->targetFacing >= 0) {
            p->facing = p->targetFacing;
            p->targetFacing = -1;
        } else {
            s32 direction = ((((angle + 16) & 255) >> 5) + 3) & 7;
            direction |= 1;
            p->facing = direction;
        }
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        ActorSequence_Begin(p, 0, 0);
        p->triggerAnimationCallback = Player_RestartAnimationFromSetup;
        PlayerCue_CompleteAction(p);
    } else {
        p->velocityX = Truncate(gUnk_086149C4[(angle + 64) & 255] * 9);
        p->velocityZ = Truncate(gUnk_086149C4[angle & 255] * 9);
        {
            u8 d = ((((angle + 16) & 255) >> 5) + 3) & 7;
            d |= 1;
            p->facing = d;
        }
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
    }
    Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->setupResource, 0,
                     p->heading, p->side);
    PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
}
