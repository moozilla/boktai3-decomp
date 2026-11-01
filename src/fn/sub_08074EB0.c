#include "scene_actor_motion.h"
#include "scene_actor_terrain_reaction.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_chase_types.h"
#include "player_cue_sequence.h"
void SceneActorMovement_UpdateAirborne(struct SceneActorMotionView *);
void SceneActorMovement_UpdateHeight(struct SceneActorRegistryActor *);
void Obj_StepPosition(void *);
struct RepeatedEffectPosition;
s32 RepeatedEffect_RequestNearby(struct RepeatedEffectPosition *);

static inline u32 SceneActorMotion_BehaviorBits(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline u32 SceneActorMotion_TestStatus(u32 *flags, u32 bits)
{
    return *flags & bits;
}
static inline u8 SceneActorMotion_HasBodyFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}
static inline void SceneActorMotion_SetFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 value = bits;
    u16 *field = &actor->flags2D4;
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

static inline u16 *SceneActorMotion_MarkMoving(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 value = bits;
    u16 *field = &actor->flags2D6;
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

static inline u16 SceneActorMotion_Elapsed(struct SceneActorRegistryResetView *actor)
{
    if (!actor->drawableKind)
        return SceneActorMotion_SequenceElapsed(
            &((struct SceneActorMotionSequence *)actor->drawable)->sequence);
    return ((struct SceneActorMotionTile *)actor->drawable)->elapsed;
}
s32 SceneActorMovement_UpdateAnimationAndTerrain(struct SceneActorRegistryActor *object)
{
    struct SceneActorRegistryResetView *actor = (void *)object;
    struct ContactCollider *collider;
    struct ContactPosition *position;
    struct ContactPosition center, delta;
    u16 *flags;
    u32 enabledBit;
    u32 category;
    {
        u8 *enabled = &actor->enabled;
        enabledBit = 1;
        *enabled = enabledBit;
    }
    if (SceneActorMotion_BehaviorBits(actor, 1))
        SceneActorMotion_SetFlags(actor, 0x40);
    if (actor->field2B3 != 1) {
        if (!actor->drawableKind) {
            struct SceneActorMotionSequence *drawable = actor->drawable;
            actor->field2B3 =
                SceneActorMotion_StepSequence(drawable, &drawable->sequence, enabledBit);
        } else {
            void *drawable = actor->drawable;
            struct DisplayKeyframeState *state = (void *)((u8 *)drawable + 0x20);
            s32 ended = Anim_Step(state, drawable);
            u8 bits = (enabledBit & state->status);
            if (bits || ended)
                actor->field2B3 = enabledBit;
        }
    }
    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return 1;
    SceneActorMovement_UpdateAirborne((void *)actor);
    if (SceneActorMotion_HasBodyFlags(actor, 0x20))
        actor->body.movementProbe = 0;
    else if (SceneActorMotion_BehaviorBits(actor, 0x2000))
        actor->body.movementProbe = (u8 *)actor + 0x40;
    else
        actor->body.movementProbe = 0;
    {
        if (actor->body.motion.x || actor->body.motion.z) {
            flags = SceneActorMotion_MarkMoving(actor, 0x2000);
            position = &actor->body.position;
        } else {
            s32 previousX = actor->previousPosition.x;
            s32 currentX = actor->body.position.x;
            flags = &actor->flags2D6;
            position = &actor->body.position;
            if (previousX != currentX) {
                s32 previousZ = actor->previousPosition.z;
                s32 currentZ = actor->body.position.z;
                if (previousZ != currentZ)
                    SceneActorMotion_SetPending(flags, 0x2000);
            }
        }
    }
    if (SceneActorPlacement_TestFlags(flags, 0x2000))
        Obj_StepPosition(&actor->body);
    actor->previousTerrainCell = actor->terrainCell;
    actor->terrainCell = SceneActorTerrain_GetCellIndex(position);
    SceneActorMovement_UpdateHeight((void *)actor);
    if (SceneActorPlacement_TestFlags(flags, 0x2000)) {
        u16 cellFlags;
        u16 *currentCell;
        s32 cell;
        category = actor->category;
        if (SceneActorTerrain_ExcludesCategory(category))
            goto done;
        currentCell = &actor->terrainCell;
        cell = *currentCell;
        cellFlags = ((struct SceneActorChaseTerrainCell *)SceneActorTerrain_GetCell(cell))->flags;
        if (*currentCell != actor->previousTerrainCell) {
            if ((cellFlags & 0x40) &&
                actor->body.position.y + (s16)actor->field2EA <= actor->groundHeight2BE) {
                void *sink;
                if (SceneActorPlacement_IsUnavailable((void *)actor))
                    goto done;
                sink = actor->field238;
                if (!sink)
                    SceneActorTerrain_InstallControl(actor, SceneActorReaction_SinkAndRemove, 0x16);
                else
                    SceneActorTerrain_InstallControl(actor, sink, 0x16);
                SceneActorTerrain_EnterState(actor, 3, 0, 0);
                MonsterInit_SetStatusFlags(actor, 0x20000);
                goto done;
            }
            if ((cellFlags & 0x80) && MonsterInit_TestWorldFlags(0x10)) {
                collider = &actor->colliderA;
                cell = *currentCell;
                SceneActorTerrain_GetCenter(cell, &center);
                SceneActorTerrain_Subtract(&delta, &center, position);
                actor->field2B6 = Math_Atan2(delta.x, delta.z) >> 5;
                actor->field2E0 = 0x20;
                collider->cost = MonsterInit_GetHealth(actor);
                actor->reactionSoundVariant = MonsterInit_GetHealth(actor);
                collider->responseParameter = 0x28;
                actor->field2D8 = 0x200;
                actor->field2E6 = 1;
                MonsterInit_SetBehaviorFlags(actor, 0x2000);
                goto done;
            }
        }
        if (category == 3 && (actor->field2CA & 1) &&
            SceneActorPlacement_ReadFlags16((void *)actor, 0x100))
            MonsterInit_SetStatusFlags(actor, 0x10000);
        if (!SceneActorMotion_Elapsed(actor)) {
            u32 mask = 0x10000;
            u32 *status = &actor->flags2D0;
            if (SceneActorMotion_TestStatus(status, mask)) {
                RepeatedEffect_RequestNearby((void *)position);
                SceneActorMotion_ClearStatus(status, 0x10000);
            }
        }
    }
done:
    SceneActorMotion_ClearPending(flags, 0x2000);
    actor->field2CA++;
    actor->previousPosition = actor->body.position;
    return 1;
}
