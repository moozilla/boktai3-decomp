#include "global.h"

#include "scene_steering_actor.h"
#include "scene_actor_terrain.h"
#include "player_cue_sequence.h"

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

static inline void SubtractPosition(struct ContactPosition *out, const struct ContactPosition *b,
                                    const struct ContactPosition *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void AddPosition(struct ContactPosition *out, const struct ContactPosition *a,
                               const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

// scope shape 90 (differences once low registers are set aside)
// confidence=low: steers an actor toward its target, advances its frame sequence, and checks the terrain cell it moves into.
void sub_0806C3A0(void *unused, struct SceneSteeringActor *p)
{
    struct ContactPosition delta;
    s32 dist2, cell, height;

    if (p->enter) {
        p->stateFrame = 0;
        p->unknown114 = 1;
        ScaledShadow_Enable(p->shadow);
        p->enter = 0;
    }
    p->unknown5[1] = -p->direction;

    SubtractPosition(&delta, &p->position, &p->target);
    dist2 = delta.x * delta.x + delta.z * delta.z;
    SceneSteeringActor_TurnTowardTarget(p);

    p->velocity.x = Sub0806C3A0_Truncate(p->speed * gUnk_086149C4[((p->direction + 0x40) & 255)]);
    p->velocity.z = Sub0806C3A0_Truncate(p->speed * gUnk_086149C4[p->direction]);

    AddPosition(&delta, &p->velocity, &p->position);
    cell = SceneActorTerrain_GetCellIndex(&delta);
    height = (*SceneActorTerrain_GetCell(cell) & 15) << 8;
    if (height >= p->position.y || height == 0) {
        p->state = 5;
        p->enter = 1;
        return;
    }

    AddPosition(&p->position, &p->position, &p->velocity);

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

    PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);

    if (p->lifetime > 0) {
        p->lifetime--;
    } else {
        p->state = 5;
        p->enter = 1;
    }
}
