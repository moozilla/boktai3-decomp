#include "global.h"

#include "scene_steering_actor.h"
#include "scene_actor_terrain.h"

// confidence=low: steers an actor toward its target, advances its frame sequence, and checks the terrain cell it moves into.
s32 Mod(s32, s32);
void SceneSteeringActor_TurnTowardTarget(struct SceneSteeringActor *);
void SceneSteeringActor_EmitRandomParticle(struct ContactPosition *);
void SceneSteeringActor_RefreshCollider(struct SceneSteeringActor *);
void ScaledShadow_Enable(void *);

static inline s32 Sub0806C3A0_Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

#ifdef NONMATCHING
// fuzzy: 68.0%
// scope shape 90 (differences once low registers are set aside)
// confidence=low: steers an actor toward its target, advances its frame sequence, and checks the terrain cell it moves into.
void sub_0806C3A0(void *unused, struct SceneSteeringActor *p)
{
    s16 delta[3];
    s16 moved[3];
    struct DisplaySequenceState *seq;
    const u16 *cmd;
    s32 dist2, cell, height;

    if (p->enter) {
        p->stateFrame = 0;
        p->unknown114 = 1;
        ScaledShadow_Enable(p->shadow);
        p->enter = 0;
    }
    p->unknown5[1] = -p->direction;

    delta[0] = p->target.x - p->position.x;
    delta[1] = p->target.y - p->position.y;
    delta[2] = p->target.z - p->position.z;
    dist2 = delta[0] * delta[0] + delta[2] * delta[2];
    SceneSteeringActor_TurnTowardTarget(p);

    p->velocity.x = Sub0806C3A0_Truncate(p->speed * gUnk_086149C4[(u8)(p->direction + 0x40)]);
    p->velocity.z = Sub0806C3A0_Truncate(p->speed * gUnk_086149C4[p->direction]);

    moved[0] = p->position.x + p->velocity.x;
    moved[1] = p->position.y + p->velocity.y;
    moved[2] = p->position.z + p->velocity.z;
    cell = SceneActorTerrain_GetCellIndex((struct ContactPosition *)moved);
    height = (*SceneActorTerrain_GetCell(cell) & 15) << 8;
    if (height >= p->position.y || height == 0) {
        p->state = 5;
        p->enter = 1;
        return;
    }

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;

    if (Mod(p->stateFrame, 6) == 0) {
        if (p->speed < p->parameter) {
            p->speed += p->unknown114;
            p->unknown114 += 4;
        } else {
            p->speed = p->parameter;
        }
    }

    if (p->lifetime <= 0x3b || (p->stateFrame > 0x28 && dist2 <= 0x3FFFF)) {
        p->velocity.y -= 3;
        p->steeringTimer = 0;
    }

    if (Mod(p->stateFrame, 8) == 0)
        SceneSteeringActor_EmitRandomParticle(&p->position);
    SceneSteeringActor_RefreshCollider(p);
    p->stateFrame++;

    seq = &p->sequence;
    cmd = seq->commands + seq->index;
    p->frame = *cmd >> 6;
    if ((seq->flags & 1) != ((*cmd & 0x30) >> 4 & 1))
        p->flags |= 4;
    else
        p->flags &= ~4;
    if ((seq->flags & 2) != ((*cmd & 0x30) >> 4 & 2))
        p->flags |= 8;
    else
        p->flags &= ~8;

    seq->elapsed++;
    if ((u16)seq->elapsed >= seq->duration) {
        seq->elapsed = 0;
        if (seq->flags & 4) {
            if (seq->index != 0) {
                seq->index = seq->index - 1;
            } else if (seq->flags & 8) {
                seq->elapsed = seq->duration;
            } else {
                seq->index = seq->frameCount - 1;
            }
        } else {
            seq->index++;
            if ((u16)seq->index >= seq->frameCount) {
                if (seq->flags & 8) {
                    seq->elapsed = seq->duration;
                    seq->index = seq->frameCount - 1;
                } else {
                    seq->index = 0;
                }
            }
        }
        cmd = seq->commands + seq->index;
        seq->frameWeight = *cmd & 15;
        seq->duration = (seq->speed * seq->frameWeight) >> 6;
        if ((u8)seq->duration == 0)
            seq->duration = 1;
    }

    if (p->lifetime > 0) {
        p->lifetime--;
    } else {
        p->state = 5;
        p->enter = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0806C3A0);
#endif
