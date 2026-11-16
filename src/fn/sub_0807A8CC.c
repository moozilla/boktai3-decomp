#include "global.h"
#include "scene_actor_motion.h"
#include "scene_actor_terrain_reaction.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_chase_types.h"
#include "player_cue_sequence.h"
void SceneActorMovement_UpdateAirborne(struct SceneActorMotionView *);
void SceneActorMovement_UpdateHeight(struct ChaseActor *);
void Obj_StepPosition(void *);
struct RepeatedEffectPosition;
s32 RepeatedEffect_RequestNearby(struct RepeatedEffectPosition *);
void NavMesh_UpdateProbe(void *, void *);
void sub_0821DE30(void *);

static inline u32 SceneActorMotion_BehaviorBits(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}
static inline u32 SceneActorMotion_TestStatus(u32 *flags, u32 bits)
{
    return *flags & bits;
}
static inline u8 SceneActorMotion_HasBodyFlags(struct ChaseActor *actor, u32 mask)
{
    if (actor->base.placementFlags & mask)
        return 1;
    return 0;
}
static inline void SceneActorMotion_SetFlags(struct ChaseActor *actor, u32 bits)
{
    u32 value = bits;
    u16 *field = &actor->base.motionFlags;
    value |= *field;
    *field = value;
}

static inline void SceneActorMotion_ClearPending(u16 *flags, u32 bits)
{
    u32 keep = ~bits;
    *flags = keep & *flags;
}

static inline u32 SceneActorMotion_StepSequence(struct SceneActorMotionSequence *drawable,
                                                struct DisplaySequenceState *input,
                                                u32 firstFlipMask)
{
    struct SceneActorMotionSequence *sprite = drawable;
    struct DisplaySequenceState *state = input;
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table = drawable->sequence.commands;
        command = (const u16 *)((u32)table + offset);
    }
    return PlayerCue_AdvanceCommand((struct ContactSwarmSprite *)sprite, state, command,
                                    firstFlipMask);
}

static inline u16 SceneActorMotion_SequenceElapsed(struct DisplaySequenceState *s)
{
    return s->elapsed;
}

static inline u16 *SceneActorMotion_MarkMoving(struct ChaseActor *actor, u32 bits)
{
    u32 value = bits;
    u16 *field = &actor->base.reactionFlags;
    value |= *field;
    *field = value;
    return field;
}

static inline void SceneActorMotion_SetPending(u16 *field, u32 bits)
{
    u32 mask = bits;
    mask |= *field;
    *field = mask;
}
static inline void SceneActorMotion_ClearStatus(u32 *f, u32 bit)
{
    u32 mask = ~bit;
    *f &= mask;
}

static inline void SceneActorMotion_SetStatus(u32 *f, u32 bit)
{
    u32 mask = bit;
    *f |= mask;
}
static inline u16 SceneActorMotion_Elapsed(struct ChaseActor *actor)
{
    if (!actor->base.drawableKind)
        return SceneActorMotion_SequenceElapsed(
            &((struct SceneActorMotionSequence *)actor->base.drawable)->sequence);
    return ((struct SceneActorMotionTile *)actor->base.drawable)->elapsed;
}
s32 SceneActorMotion_Update(struct ChaseActor *object)
{
    struct ChaseActor *actor = (void *)object;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    struct ContactCollider *collider;
    struct ContactPosition *position;
    struct ContactPosition center, delta;
    u32 *behavior;
    u16 *flags;
    u32 enabledBit;
    u32 category;
    {
        u8 *enabled = &actor->base.enabled;
        enabledBit = 1;
        *enabled = enabledBit;
    }
    if (SceneActorMotion_BehaviorBits(actor, 1))
        SceneActorMotion_SetFlags(actor, 0x40);
    if (actor->base.restart != 1) {
        if (!actor->base.drawableKind) {
            struct SceneActorMotionSequence *drawable = actor->base.drawable;
            actor->base.restart =
                SceneActorMotion_StepSequence(drawable, &drawable->sequence, enabledBit);
        } else {
            void *drawable = actor->base.drawable;
            struct DisplayKeyframeState *state = (void *)((u8 *)drawable + 0x20);
            s32 ended = Anim_Step(state, drawable);
            u8 bits = (enabledBit & state->status);
            if (bits || ended)
                actor->base.restart = enabledBit;
        }
    }
    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return 1;
    SceneActorMovement_UpdateAirborne((void *)actor);
    {
        u32 mask = 0x2000;
        u32 *field = &actor->base.behaviorFlags;
        u32 moving = *field & mask;
        behavior = field;
        if (moving && !SceneActorMotion_HasBodyFlags(actor, 0x20))
            actor->base.body.movementProbe = (u8 *)actor + 0x40;
        else
            actor->base.body.movementProbe = 0;
    }
    {
        if (actor->base.body.motion.x || actor->base.body.motion.z) {
            flags = SceneActorMotion_MarkMoving(actor, 0x2000);
            position = &actor->base.body.position;
        } else {
            s32 previousX = actor->base.previousPosition.x;
            s32 currentX = actor->base.body.position.x;
            flags = &actor->base.reactionFlags;
            position = &actor->base.body.position;
            if (previousX == currentX) {
                s32 previousZ = actor->base.previousPosition.z;
                s32 currentZ = actor->base.body.position.z;
                if (previousZ == currentZ)
                    SceneActorMotion_SetPending(flags, 0x2000);
            }
        }
    }
    if (SceneActorPlacement_TestFlags(flags, 0x2000))
        Obj_StepPosition(&actor->base.body);
    actor->base.previousTerrainCell = actor->base.terrainCell;
    actor->base.terrainCell = SceneActorTerrain_GetCellIndex(position);
    SceneActorMovement_UpdateHeight((void *)actor);
    if (SceneActorPlacement_TestFlags(flags, 0x2000)) {
        u16 cellFlags;
        u16 *currentCell;
        s32 cell;
        category = actor->base.category;
        if (SceneActorTerrain_ExcludesCategory(category))
            goto done;
        currentCell = &actor->base.terrainCell;
        cell = *currentCell;
        cellFlags = ((struct SceneActorChaseTerrainCell *)SceneActorTerrain_GetCell(cell))->flags;
        if (*currentCell != actor->base.previousTerrainCell) {
            if ((cellFlags & 0x40) && actor->base.body.position.y + (s16)actor->base.heightOffset <=
                                          actor->base.groundHeight) {
                void *sink;
                if (SceneActorPlacement_IsUnavailable((void *)actor))
                    goto done;
                sink = actor->base.sinkCallback;
                if (!sink)
                    SceneActorTerrain_InstallControl(actor, SceneActorReaction_SinkAndRemove, 0x16);
                else
                    SceneActorTerrain_InstallControl(actor, sink, 0x16);
                SceneActorTerrain_EnterState(actor, 3, 0, 0);
                MonsterInit_SetStatusFlags(actor, 0x20000);
                goto done;
            }
            if ((cellFlags & 0x80) && MonsterInit_TestWorldFlags(0x10)) {
                collider = &actor->base.colliderA;
                cell = *currentCell;
                SceneActorTerrain_GetCenter(cell, &center);
                SceneActorTerrain_Subtract(&delta, &center, position);
                actor->base.reactionDirection = Math_Atan2(delta.x, delta.z) >> 5;
                actor->base.hitOwners = 0x20;
                collider->cost = MonsterInit_GetHealth(actor);
                actor->base.reactionSoundVariant = MonsterInit_GetHealth(actor);
                collider->responseParameter = 0x28;
                actor->base.hitAttributes = 0x200;
                actor->base.field2E6 = 1;
                SceneActorMotion_SetStatus(behavior, 0x2000);
                goto done;
            }
        }
        if (category == 3 && (actor->base.frameCounter & 1) &&
            SceneActorPlacement_ReadFlags16((void *)actor, 0x100))
            MonsterInit_SetStatusFlags(actor, 0x10000);
        if (!SceneActorMotion_Elapsed(actor)) {
            u32 mask = 0x10000;
            u32 *status = &actor->base.statusFlags;
            if (SceneActorMotion_TestStatus(status, mask)) {
                RepeatedEffect_RequestNearby((void *)position);
                SceneActorMotion_ClearStatus(status, 0x10000);
            }
        }
    }
done: {
    void *contact = ((struct SceneActorMotionTerrainGroup *)group)->contact;
    if (SceneActorMotion_TestStatus(behavior, 0x400000)) {
        u16 cellFlags = ((struct SceneActorChaseTerrainCell *)SceneActorTerrain_GetCell(
                             actor->base.terrainCell))
                            ->flags;
        if ((cellFlags & 3) != 3 && !(cellFlags & 0x440))
            NavMesh_UpdateProbe(contact, position);
    }
}
    actor->base.frameCounter++;
    sub_0821DE30(group);
    actor->base.previousPosition = actor->base.body.position;
    SceneActorMotion_ClearPending(flags, 0x2000);
    return 1;
}
