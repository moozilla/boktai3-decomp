#include "global.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_chase_step.h"
#include "scene_actor_reaction.h"
#include "scene_actor_init_primitives.h"
#include "display_sequence.h"

struct GhoulDiscContext {
    u8 unknown0[0x624];
    u16 field624, field626, flags;
    u16 unknown62A;
    struct ContactPosition targetPositions[2];
    u16 unknown63C, field63E;
    u8 discKind;
};

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);
void SunDiscPool_SpawnDisc(const struct ContactPosition *, const struct ContactPosition *, s32, s32,
                           u32, s32, u32, s32, s32);

static inline u8 Ghoul_TestDiscFlags(struct GhoulDiscContext *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}

static inline void Ghoul_ClearDiscFlags(struct GhoulDiscContext *context, u32 mask)
{
    context->flags &= ~mask;
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

static inline s32 Ghoul_DirectionDelta(s32 direction, s32 target)
{
    s32 delta = target - direction;
    if (delta > 128)
        delta -= 256;
    else if (delta < -128)
        delta += 256;
    return delta;
}

static inline s32 Ghoul_TargetPlayerSlot(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTarget *target = actor->tracking.target;
    if (!target)
        return -1;
    return SceneActorChase_PlayerSlot(target);
}

static inline void Ghoul_CaptureTarget(struct SceneActorRegistryResetView *actor,
                                       struct GhoulDiscContext *context, u32 slot)
{
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        s32 player = Ghoul_TargetPlayerSlot(actor);
        if (player >= 0)
            context->targetPositions[slot] =
                ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->target.position;
    } else {
        context->targetPositions[slot] = actor->tracking.target->position;
    }
}

static inline s32 Ghoul_MapWidth(struct EngineMap **terrain)
{
    return ((struct SceneActorChaseTerrainGrid *)(*terrain)->cells)->width;
}

static inline s32 Ghoul_ForwardCell(struct SceneActorRegistryResetView *actor, s32 steps)
{
    struct EngineMap **terrain;
    u32 mapWord;
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
                mapWord = (u32)&gTerrainMap;
                mapWord = *(u32 *)mapWord;
                stride = ((struct SceneActorChaseTerrainGrid *)((struct EngineMap *)mapWord)->cells)
                             ->width;
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
                mapWord = (u32)&gTerrainMap;
                mapWord = *(u32 *)mapWord;
                stride = ((struct SceneActorChaseTerrainGrid *)((struct EngineMap *)mapWord)->cells)
                             ->width;
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
    struct GhoulDiscContext *context = actor->context;
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
            SceneActorTerrain_GetCenter(cell, &context->targetPositions[1]);
            {
                switch (delta = Ghoul_QuarterDirection(actor->body.direction + 32) & 3) {
                    case 0:
                        context->targetPositions[1].x += 127;
                        context->targetPositions[1].z = actor->body.position.z;
                        break;
                    case 1:
                        context->targetPositions[1].x = actor->body.position.x;
                        context->targetPositions[1].z += 127;
                        break;
                    case 2:
                        context->targetPositions[1].x -= 127;
                        context->targetPositions[1].z = actor->body.position.z;
                        break;
                    case 3:
                        context->targetPositions[1].x = actor->body.position.x;
                        context->targetPositions[1].z -= 127;
                        break;
                }
                {
                    struct ContactPosition *position = &context->targetPositions[1];
                    context->targetPositions[1].y =
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
            SunDiscPool_SpawnDisc(&origin, &context->targetPositions[1], first, second, flags,
                                  parameter, context->discKind, 30, -20);
        }
        Sound_PlaySong(0xD6);
    }
    if (actor->field2B3)
        actor->field2B0 = 1;
}
