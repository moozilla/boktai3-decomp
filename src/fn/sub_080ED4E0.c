#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "display_sequence_inline.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"

struct IvyTarget {
    u8 unk0[8];
    u8 kind;
    u8 unk9[0x3C - 9];
    struct SceneActorRegistryResetView *actor;
};
struct IvyContext {
    u8 unknown0[0x62C];
    u32 flags;
};
struct Player;
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void SceneActorReaction_ApplyHitResponse(void *, u32, struct ContactPosition *, u32, u32, u32);
s32 Ivy_FindAdjacentTarget(struct SceneActorRegistryResetView *, s32);
void Ivy_UpdateHold(void);
void SceneActorReaction_FinishAnimation17(struct SceneActorRegistryResetView *);
struct IvyOffset {
    u16 x, y, z, pad;
};
extern const struct IvyOffset gUnk_08606384[2][5];

static inline void Ivy_Facing(struct SceneActorRegistryResetView *actor, u8 *quadrant, u8 *flip)
{
    s16 side = ((actor->body.direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

static inline void Ivy_EnableCollider(struct ContactCollider *collider)
{
    collider->flags |= 4;
    collider->flags &= 0xFFF7;
}
static inline void Ivy_SetContactFlag(struct SceneActorRegistryResetView *actor, u32 bits)
{
    actor->flags2C0 = bits | actor->flags2C0;
}
static inline s32 Ivy_GetElapsed(struct DisplaySequenceState *sequence)
{
    return sequence->elapsed;
}
static inline void Ivy_DisableCollider(struct ContactCollider *collider)
{
    collider->flags &= 0xFFFB;
    collider->flags |= 8;
}
static inline void Ivy_DisableContact(struct SceneActorRegistryResetView *actor)
{
    Ivy_DisableCollider(&actor->colliderA);
    SceneActorChase_ClearFlagComponent((void *)&actor->flags2C0, 0x800);
}

struct IvyFrameIndex {
    u16 index;
};
static inline struct IvyFrameIndex Ivy_ReadFrame(struct DisplaySequenceState *sequence)
{
    struct IvyFrameIndex value = {0};
    value.index = sequence->index;
    return value;
}

void sub_080ED4E0(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *anim;
    struct IvyTarget *target;
    struct ContactPosition pos;
    u8 quadrant, flip;
    u32 started;

    anim = &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    started = SceneActorReaction_TakePrepared(actor);
    if (started)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    if (anim->index == 3 && anim->elapsed == 0)
        Sound_PlaySong(0x1DD);

    if (Ivy_ReadFrame(anim).index == 3 && Ivy_GetElapsed(anim) > 6) {
        actor->colliderA.flags |= 4;
        actor->colliderA.flags &= 0xFFF7;
        Ivy_SetContactFlag(actor, 0x800);
        if (Ivy_FindAdjacentTarget(actor, 0)) {
            SceneActorChase_SetControl(actor, Ivy_UpdateHold, 0x25);
            SceneActorChase_SetState(actor, 0, 2, 1);

            Ivy_Facing(actor, &quadrant, &flip);
            pos.x = actor->body.position.x + gUnk_08606384[flip][anim->index].x;
            pos.y = actor->body.position.y + gUnk_08606384[flip][anim->index].y;
            pos.z = actor->body.position.z + gUnk_08606384[flip][anim->index].z;
            target = (void *)actor->tracking.target;
            if (target->kind <= 3) {
                Player_BeginPositionRequest((void *)gUnknown_02000580[target->kind], (void *)&pos,
                                            0, 0, flip, 0, 0, 0, 0);
            } else if (target->kind == 4) {
                struct SceneActorRegistryResetView *t = target->actor;
                t->colliderA.responseValue = 0;
                SceneActorTerrain_ClearCollisionTimers((void *)t);
                SceneActorChase_ClearMotionFlagComponent(t, 0x20);
                SceneActorReaction_ApplyHitResponse(t, 0, (void *)&pos, 0, flip, 0);
            }
            {
                struct IvyContext *context = actor->context;
                u32 mask = 2;
                context->flags |= mask;
            }
            return;
        }
    } else if (Ivy_ReadFrame(anim).index <= 1) {
        Ivy_DisableContact(actor);
    } else {
        Ivy_EnableCollider(&actor->colliderA);
        Ivy_SetContactFlag(actor, 0x800);
    }

    if (actor->field2B3) {
        Ivy_DisableContact(actor);
        SceneActorChase_SetControl(actor, SceneActorReaction_FinishAnimation17, 0x27);
        SceneActorChase_SetState(actor, 0, 2, 3);
    }
}
