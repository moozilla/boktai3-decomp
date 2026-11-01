#include "global.h"
#ifdef NONMATCHING
// fuzzy: 79.1%
#include "scene_actor_chase_reaction.h"
#include "scene_actor_straight_route.h"

#include "scene_actor_cardinal_path.h"

static inline u8 FindPath(struct SceneActorRegistryResetView *actor, s32 cell, s32 goal)
{
    s32 y = actor->body.position.y;
    u32 kind = *SceneActorTerrain_GetCell(cell) >> 4;
    s32 total = SceneActorPath_Width() * SceneActorPath_Height();
    return SceneActorPath_CanReach(cell, goal, y, kind, total);
}
static inline u32 Chase_RandomBits(u32 mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return gRandomTable[index] & mask;
}
static inline void Chase_BeginTurn(void *obj, u8 saved, u32 duration)
{
    struct SceneActorRegistryResetView *actor = obj;
    struct SceneActorChaseGroup *group = &actor->group;
    if (!SceneActorChase_MotionFlags(actor, 0x400) && !SceneActorChase_Test(actor->group.common.gate.flags, 4)) {
        s32 prior = actor->body.direction;
        s32 tries = 0;
        s16 choice;
        do {
            choice = Chase_RandomBits(3) << 6;
            if (choice != prior)
                goto chosen;
        } while (++tries <= 3);
        choice = prior;
    chosen:
        group->turn.direction = choice;
        SceneActorChase_SaveControl(&actor->group.saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = duration;
    }
}

// confidence=low: finds a cardinal recovery destination and resumes route movement.
#define SceneActorMovement_SelectRecoveryRoute sub_0807AF94
u8 SceneActorMovement_SelectRecoveryRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget *subject;
    s32 direction, cell, target;
    struct ContactPosition center;
    if (SceneActorChase_Test(actor->group.common.gate.flags, 2))
        return 0;
    if (!SceneActorChase_Test(group->common.gate.flags, 1))
        return 0;
    subject = (void *)actor->tracking.target;
    if (actor->body.position.y != subject->position.y) {
        SceneActorChase_Arrive(actor);
        group->common.gate.flags |= 2;
        return 1;
    }
    switch (((actor->body.direction + 32) >> 6) & 3) {
        case 0:
        case 2:
            cell = actor->terrainCell;
            if (FindPath(actor, cell, cell - SceneActorPath_Width())) {
                direction = 3;
                break;
            }
            cell = actor->terrainCell;
            if (FindPath(actor, cell, cell + SceneActorPath_Width())) {
                direction = 1;
                break;
            }
            direction = -1;
            break;
        case 1:
        case 3:
            cell = actor->terrainCell;
            if (FindPath(actor, cell, cell + 1)) {
                direction = 0;
                break;
            }
            cell = actor->terrainCell;
            if (FindPath(actor, cell, cell - 1)) {
                direction = 2;
                break;
            }
            direction = -1;
            break;
        default:
            direction = -1;
            break;
    }
    if (direction < 0)
        return 0;
    target = SceneActorTerrain_LastOpenCell(actor, direction);
    if (!target || target == actor->terrainCell)
        return 0;
    Chase_BeginTurn(actor, actor->body.direction, 2);
    SceneActorTerrain_GetCenter(target, &center);
    center.y = actor->body.position.y;
    if (SceneActorChase_TryRoute(actor, group->route, &actor->body.position, &center)) {
        group->common.gate.flags |= 2;
        return 1;
    }
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
    SceneActorChase_SetBehavior(actor, 3);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807AF94);
#endif
