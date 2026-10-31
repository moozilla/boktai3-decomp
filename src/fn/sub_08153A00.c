#include "player_cue.h"
#include "player_cue_sequence.h"
#include "render.h"
// confidence=low: tentative player cue role; see callback body.

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
static inline s32 Trunc6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -((-v) >> 6);
}

void Player_RestartAnimationFromResource(struct Player *);
void Player_UpdateFacingSide(struct Player *);
u32 Player_SetAnimation(struct Player *, u32, u32, u32);
void SpiralTrail_CreateWithParticles(struct Player *, s32, void *, s32, s32);
static inline void PlayerCue_FaceSideWalkAngle(struct Player *p, u16 angle)
{
    s32 shifted = (angle + 112) >> 5;
    u32 mask = 7;
    u8 direction;
    u8 *facing;
    mask &= shifted;
    facing = &p->facing;
    direction = mask;
    *facing = mask;
    if (direction > 4) {
        p->heading = 8 - direction;
        p->side = 1;
    } else {
        u8 *heading = &p->heading;
        *heading = mask;
        p->side = 0;
    }
}

// confidence=low: state-four variant uses eight-facing animation and axis-dependent speed.
void PlayerCue_UpdateSideWalkTarget(struct Player *p)
{
    s32 dx = PlayerCue_Motion(p)->destination.x - p->position.x,
        dy = PlayerCue_Motion(p)->destination.z - p->position.z;
    s32 angle = Math_Atan2(dx, dy);
    if (dx * dx + dy * dy <= 144) {
        p->position.x = PlayerCue_Motion(p)->destination.x;
        p->position.z = PlayerCue_Motion(p)->destination.z;
        if (p->targetFacing >= 0) {
            p->facing = p->targetFacing;
            p->targetFacing = -1;
        } else {
            s32 shifted = ((u16)angle + 112) >> 5;
            u32 mask = 7;
            mask &= shifted;
            p->facing = mask;
        }
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
        Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource, 0,
                         p->heading, p->side);
        PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
        ActorSequence_Begin(p, 0, 0);
        p->triggerAnimationCallback = Player_RestartAnimationFromResource;
        PlayerCue_CompleteAction(p);
    } else {
        p->velocityX = Truncate(gUnk_086149C4[(angle + 64) & 255] * 12);
        p->velocityZ = Truncate(gUnk_086149C4[angle & 255] * 12);
        if (p->velocityScaleAxis == 1) {
            s32 v;
            if (p->velocityZ < 0) {
                v = p->velocityZ * 40;
                p->velocityZ = Trunc6(v);
            } else {
                v = p->velocityZ * 52;
                p->velocityZ = Trunc6(v);
            }
        } else if (p->velocityScaleAxis == 2) {
            s32 v;
            if (p->velocityX < 0) {
                v = p->velocityX * 40;
                p->velocityX = Trunc6(v);
            } else {
                v = p->velocityX * 52;
                p->velocityX = Trunc6(v);
            }
        }
        PlayerCue_FaceSideWalkAngle(p, angle);
        Anim_SetSequence(&PlayerCue_Graphics(p)->sequence, PlayerCue_Graphics(p)->sideResource, 1,
                         p->heading, p->side);
        PlayerCue_StepSequence(&PlayerCue_Graphics(p)->sprite, &PlayerCue_Graphics(p)->sequence);
    }
}
