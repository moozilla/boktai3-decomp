// MATCHING_CONCESSION: register-pinning
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

static inline s32 ScaleSequenceWeight(s32 weight, s32 speed)
{
    return (speed * weight) >> 6;
}

// scope shape 90 (differences once low registers are set aside)
// confidence=low: steers an actor toward its target, advances its frame sequence, and checks the terrain cell it moves into.
void sub_0806C3A0(void *unused, struct SceneSteeringActor *p)
{
    struct ContactPosition delta;
    struct DisplaySequenceState *seq;
    const volatile u16 *cmd;
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

    {
        u32 ended, flags, bit;
        register u32 duration asm("r2");
        seq = &p->sequence;
        cmd = (const volatile u16 *)((u32)p->sequence.commands + (seq->index << 1));
        p->frame = *cmd >> 6;
        {
            u32 input = seq->flags;
            register u32 first asm("r1");
            u32 maskBit = 1;
            bit = maskBit;
            asm("" : "+r"(bit));

            first = bit;
            first &= input;
            {
                u32 bits = *cmd;
                u32 flip = 0x30;
                flip &= bits;
                flip >>= 4;
                flip &= bit;

                if (first != flip)
                    p->flags |= 4;
                else
                    p->flags &= ~4;
            }
        }
        {
            u32 input = seq->flags;
            u32 bit;
            register u32 first asm("r1");
            bit = 2;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            asm("" : "+r"(first));
            first = (u8)first;
            if (first != (((*cmd & 0x30) >> 4) & bit))
                p->flags |= 8;
            else
                p->flags &= ~8;
        }
        {
            u32 elapsed = seq->elapsed + 1;
            u32 mask;
            register u32 temp asm("r1");
            register u32 savedDuration asm("r3");
            seq->elapsed = elapsed;
            temp = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            duration = seq->duration;
            elapsed = (u16)elapsed;
            savedDuration = duration;
            if (elapsed >= duration) {
                seq->elapsed = 0;
                flags = seq->flags;
                if (flags & 4) {
                    u32 index = seq->index;
                    if (index == 0) {
                        u32 hold = 8;
                        hold &= flags;
                        if (hold) {
                            seq->elapsed = savedDuration;
                        } else {
                            seq->index = seq->frameCount - 1;
                        }
                        ended = 1;
                    } else {
                        seq->index = index - 1;
                        ended = 0;
                    }
                } else {
                    u32 index = seq->index + 1;
                    u32 frameCount;
                    u32 endFlag;
                    seq->index = index;
                    frameCount = seq->frameCount;
                    if ((index & mask) >= frameCount) {
                        endFlag = flags & 8;
                        asm("" : "+r"(endFlag));
                        endFlag = (u8)endFlag;
                        if (endFlag) {
                            seq->elapsed = duration;
                            {
                                u32 lastIndex = frameCount - 1;
                                seq->index = lastIndex;
                            }
                        } else {
                            seq->index = 0;
                        }
                        ended = 1;
                    } else {
                        ended = 0;
                    }
                }
                {
                    u32 offset = seq->index << 1;
                    const volatile u16 *table;
                    table = seq->commands;
                    cmd = (const volatile u16 *)((u32)table + offset);
                }
                seq->frameWeight = *cmd & 15;
                {
                    s32 weight = seq->frameWeight;
                    s32 speed;
                    speed = seq->speed;
                    speed = ScaleSequenceWeight(weight, speed);
                    seq->duration = speed;
                    if (!(u8)speed)
                        seq->duration = 1;
                }
            } else {
                ended = 0;
            }
        }
    }

    if (p->lifetime > 0) {
        p->lifetime--;
    } else {
        p->state = 5;
        p->enter = 1;
    }
}
