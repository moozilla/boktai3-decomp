#include "ghoul.h"
#include "scene_actor_availability.h"
#include "global.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_chase_step.h"
#include "scene_actor_reaction.h"
#include "scene_actor_init_primitives.h"
#include "display_sequence.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_turn.h"

void Ghoul_OnAttackCollision(struct ContactCollider *unused, struct ContactCollider *contact,
                             struct SceneActorRegistryResetView *actor)
{
    int mask;
    u32 *flags;
    if ((SceneActor_ActionsBlocked((void *)actor) == 0) && (contact->contactFlags & 0xF)) {
        actor->field304 = contact->collisionId;
        flags = &actor->flags2D0;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

void SceneActorReaction_HandleColliderContact(void);

void Ghoul_OnBodyCollision(void)
{
    SceneActorReaction_HandleColliderContact();
}

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

void SunDiscPool_SpawnDisc(const struct ContactPosition *, const struct ContactPosition *, s32, s32,
                           u32, s32, u32, s32, s32);

static inline u8 Ghoul_TestDiscFlags(struct Ghoul *context, u32 mask)
{
    if (context->discFlags & mask)
        return 1;
    return 0;
}

static inline void Ghoul_ClearDiscFlags(struct Ghoul *context, u32 mask)
{
    context->discFlags &= ~mask;
}

static inline s32 Ghoul_QuarterDirection(s32 angle)
{
    if (angle >= 0)
        return angle >> 6;
    return -(-angle >> 6);
}

static inline s32 Ghoul_Abs(s32 value)
{
    if (value < 0)
        return -value;
    return value;
}

static inline s32 Ghoul_TargetPlayerSlot(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTarget *target = actor->tracking.target;
    if (!target)
        return -1;
    return SceneActorChase_PlayerSlot(target);
}

static inline void Ghoul_CaptureTarget(struct SceneActorRegistryResetView *actor,
                                       struct Ghoul *context, u32 slot)
{
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        s32 player = Ghoul_TargetPlayerSlot(actor);
        if (player >= 0)
            context->discTargets[slot] =
                ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->target.position;
    } else {
        context->discTargets[slot] = actor->tracking.target->position;
    }
}

static inline s32 Ghoul_MapWidth(struct EngineMap **terrain)
{
    return ((struct SceneActorChaseTerrainGrid *)(*terrain)->cells)->width;
}

static inline s32 Ghoul_ForwardCell(struct SceneActorRegistryResetView *actor, s32 steps)
{
    struct EngineMap **terrain;
    u32 cell = actor->terrainCell;
    u32 height = SceneActorChase_HeightAt(cell, &actor->body.position);
    u32 shape = SceneActorChase_CellProperties(cell)->attributes >> 4;
    s32 next, i, chosen, stride;
    switch (Ghoul_QuarterDirection(actor->body.direction + 32) & 3) {
        case 0:
            next = cell + 1;
            i = 0;
            do {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    break;
                next++;
                i++;
            } while (i < steps);
            chosen = next - 1;
            break;
        case 1:
            terrain = &gTerrainMap;
            next = cell + Ghoul_MapWidth(terrain);
            for (i = 0; i < steps; i++) {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    goto blockedSouth;
                stride = ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width;
                next += stride;
            }
            chosen = next - stride;
            break;
        case 2:
            next = cell - 1;
            i = 0;
            do {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    break;
                next--;
                i++;
            } while (i < steps);
            chosen = next + 1;
            break;
        case 3:
            terrain = &gTerrainMap;
            next = cell - Ghoul_MapWidth(terrain);
            for (i = 0; i < steps; i++) {
                if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                    goto blockedNorth;
                stride = ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width;
                next -= stride;
            }
            chosen = next + stride;
            break;
        blockedSouth:
            terrain = &gTerrainMap;
            chosen = next - Ghoul_MapWidth(terrain);
            break;
        blockedNorth:
            terrain = &gTerrainMap;
            chosen = next + Ghoul_MapWidth(terrain);
            break;
        default:
            chosen = actor->terrainCell;
            break;
    }
    return chosen;
}

void Ghoul_UpdateAimedDiscThrow(struct SceneActorRegistryResetView *actor)
{
    struct Ghoul *context = actor->context;
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->body.direction = (((actor->tracking.heading + 32) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        if (Ghoul_TestDiscFlags(context, 4)) {
            sequence->index = 2;
            sequence->elapsed = 0;
            Ghoul_ClearDiscFlags(context, 4);
        }
        Ghoul_CaptureTarget(actor, context, 0);
    }
    if (sequence->index == 5 && sequence->elapsed == 0) {
        s32 direction = (((actor->body.direction + 32) & 255) >> 6) << 6;
        s32 delta = actor->tracking.heading - direction;
        struct ContactPosition origin;
        if (delta > 128)
            delta -= 256;
        else if (delta < -128)
            delta += 256;
        if (Ghoul_Abs(delta) < 32) {
            Ghoul_CaptureTarget(actor, context, 1);
        } else {
            s32 cell = Ghoul_ForwardCell(actor, 1);
            SceneActorTerrain_GetCenter(cell, &context->discTargets[1]);
            {
                switch (delta = Ghoul_QuarterDirection(actor->body.direction + 32) & 3) {
                    case 0:
                        context->discTargets[1].x += 127;
                        context->discTargets[1].z = actor->body.position.z;
                        break;
                    case 1:
                        context->discTargets[1].x = actor->body.position.x;
                        context->discTargets[1].z += 127;
                        break;
                    case 2:
                        context->discTargets[1].x -= 127;
                        context->discTargets[1].z = actor->body.position.z;
                        break;
                    case 3:
                        context->discTargets[1].x = actor->body.position.x;
                        context->discTargets[1].z -= 127;
                        break;
                }
                {
                    struct ContactPosition *position = &context->discTargets[1];
                    context->discTargets[1].y =
                        SceneActorChase_HeightAt(MonsterInit_CellIndex(position), position);
                }
            }
        }
        origin = actor->body.position;
        origin.y += 220;
        {
            u32 first = actor->attacks.first[1];
            u32 second = actor->attacks.second[1];
            u32 flags = actor->attacks.flags[1];
            u32 parameter = actor->attacks.parameter[1];
            SunDiscPool_SpawnDisc(&origin, &context->discTargets[1], first, second, flags,
                                  parameter, context->discKind, 30, -20);
        }
        Sound_PlaySong(0xD6);
    }
    if (actor->field2B3)
        actor->field2B0 = 1;
}

void Ghoul_UpdateIdleState(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2 != 0)
        actor->field2B2 = 0;
}

void Ghoul_UpdateStandingState(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2 != 0)
        actor->field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
}

u32 Route_GetCurrentPointFacing(void *);

/* Turn toward the route-facing octant, then restore the waiting controller. */

void Ghoul_TurnTowardRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorBody *motion = &actor->body;
    struct SceneActorRegistryRoute *route = &actor->group.homeRoute;
    struct SceneActorChaseGroup *group = &actor->group;
    struct DisplaySequenceState *sequence;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(actor);
    s32 direction, target, delta;

    motionFlags = &actor->flags2D4;
    if (prepared) {
        actor->body.direction = (SceneActorTurn_Cardinal(actor->body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(actor->body.direction);
        if (actor->behavior2AA != 6 && actor->behavior2AA != 17 &&
            !SceneActorTurn_Flags(motionFlags, 0x4480)) {
            target = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            actor->unknown3CD[0] = target;
        } else {
            target = actor->unknown3CD[0];
        }
        if (direction == (target >> 1)) {
            actor->field2B0 = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                SceneActorChase_SetState(actor, 3, 1, 0);
                SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            }
            return;
        }
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 0, 4);
                    SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 9, 2, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 4);
                    SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 5);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 2, 4);
                    SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 9, 0, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 1);
                    SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x10000);
                }
                break;
        }
        actor->field2B3 = 0;
    }
    sequence = ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
    if (sequence->elapsed == 0 && (sequence->index == 1 || sequence->index == 3)) {
        s32 targetDirection = SceneActorTurn_OctantDirection(actor->unknown3CD[0]);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x20)
            motion->direction = targetDirection;
        else if (SceneActorTurn_ActorFlags(actor, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    SceneActorTurn_ApplyDuration(actor, motionFlags);
    if (actor->field2B3) {
        s32 current = ((actor->body.direction + 0x70) >> 5) & 7;
        delta = actor->unknown3CD[0] - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            actor->field2B0 = 1;
            actor->body.direction = (SceneActorTurn_Cardinal(actor->body.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(actor, 3, 1, 0);
                SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(actor, actor->entryTarget224, 2);
        }
    }
}

void SceneActorAttachment_ReplaceResponseSlot(void *, s32, s32, u32);

void Ghoul_UpdateDyingState(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 blockingFlags;
    u32 mask;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 8);
        test = 2;
        flags = &actor->flags2D4;
        value = *flags;
        test &= value;
        if (test == 0) {
            test = 1;
            test |= value;
            *flags = test;
        }
    }
    if (Mod(frame, 60) == 0) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 2, 0, actor->category);
    }
    if ((s16)group->field5C6 <= 0) {
        mask = 0x80;
        mask <<= 5;
        blockingFlags = actor->flags2D0 & mask;
        if (blockingFlags == 0) {
            SceneActorChase_SetControl(actor, Ghoul_UpdateDeathAnimation, 6);
        }
    }
}

void Ghoul_UpdateDeathAnimation(struct SceneActorRegistryResetView *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        s32 mask;
        u16 *motionFlags;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
        motionFlags = &actor->flags2D4;
        mask = -2;
        *motionFlags = mask & *motionFlags;
    }
    if (frame == 0x28) {
        u16 flags = 0x10;
        actor->flags2D6 = flags | actor->flags2D6;
    }
    if (actor->field2B3 && frame > 0x78)
        actor->field2B0 = 1;
}

void Bat_RestoreSavedState(void);

void Ghoul_UpdateHiddenRecovery(struct SceneActorRegistryResetView *actor, s32 frame, u32 unused)
{
    u32 queued = SceneActorReaction_TakePrepared(actor);
    if (queued) {
        u32 mask = 1;
        if (actor->drawableKind == 0) {
            struct SceneActorControlMotionDrawable *drawable = actor->drawable;
            u32 value = drawable->flags;
            drawable->flags = value | mask;
        } else {
            struct SceneActorControlMotionDrawable *drawable = actor->drawable;
            struct SceneActorAnimationTile *tile =
                &((struct SceneActorAnimationTileDrawable *)drawable)->tile;
            u32 value = tile->flags;
            tile->flags = value | mask;
        }
        ActorShadow_Disable(actor->shadow);
    } else if (frame > 0x1A) {
        void *control;
        u32 code;
        u32 *flags;
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 9);
        control = Bat_RestoreSavedState;
        code = 0xD;
        actor->field2B2 = 1;
        actor->field2B0 = queued;
        actor->field2B5 = code;
        actor->target = control;
        actor->field2C8 = queued;
        flags = &actor->flags114;
        mask = ~1;
        *flags &= mask;
        actor->word104 = queued;
    } else {
        actor->field2B3 = queued;
    }
}
