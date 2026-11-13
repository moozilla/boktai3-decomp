#include "actor_controller.h"

s32 DirectionalActor_TryBeginState(struct SceneCombatActor *actor, u32 state)
{
    if (!actor->actionReady)
        return 0;
    if (actor->action == state && actor->actionPhase != 0xff)
        return 0;
    actor->action = state;
    actor->actionPhase = 0;
    actor->actionCounter = 0;
    actor->colliderGroup.colliders[0].flags |= 4;
    actor->flags &= 0xfffd;
    return 1;
}

s32 Armor_TryStartAction(struct SceneCombatActor *actor, s32 action)
{
    s32 started;
    if (actor->actionReady) {
        actor->action = action;
        actor->actionPhase = 0;
        actor->actionCounter = 0;
        actor->colliderGroup.colliders[0].flags |= 4;
        actor->flags &= 0xFFFD;
        started = 1;
    } else {
        started = 0;
    }
    return started;
}

s32 ActorController_ForceAction(struct SceneCombatActor *actor, u8 action)
{
    actor->action = action;
    actor->actionPhase = 0;
    actor->actionCounter = 0;
    actor->colliderGroup.colliders[0].flags |= 4;
    actor->flags &= 0xFFFD;
    return 1;
}

u32 ActorController_StartTurn(struct SceneCombatActor *actor, u32 heading)
{
    s32 delta;
    actor->targetHeading = heading;
    delta = (actor->targetHeading - actor->heading) & 0xff;
    if (delta <= 0x80) {
        return DirectionalActor_TryBeginState(actor, 2);
    }
    return DirectionalActor_TryBeginState(actor, 3);
}

u32 ActorController_StartModeTwoTurn(struct SceneCombatActor *actor, u32 heading)
{
    s32 delta;
    actor->targetHeading = heading;
    delta = (actor->targetHeading - actor->heading) & 0xff;
    if (delta <= 0x80) {
        return DirectionalActor_TryBeginState(actor, 0x17);
    }
    return DirectionalActor_TryBeginState(actor, 0x18);
}

u32 ActorController_StartModeThreeTurn(struct SceneCombatActor *actor, u32 heading)
{
    s32 delta;
    actor->targetHeading = heading;
    delta = (actor->targetHeading - actor->heading) & 0xff;
    if (delta <= 0x80) {
        return DirectionalActor_TryBeginState(actor, 0x27);
    }
    return DirectionalActor_TryBeginState(actor, 0x28);
}

u32 ActorController_StartModeOneTurn(struct SceneCombatActor *actor, u32 heading)
{
    s32 delta;
    actor->targetHeading = heading;
    delta = (actor->targetHeading - actor->heading) & 0xff;
    if (delta <= 0x80) {
        return DirectionalActor_TryBeginState(actor, 0x14);
    }
    return DirectionalActor_TryBeginState(actor, 0x15);
}

u32 ActorController_StartAlternateTurn(struct SceneCombatActor *actor, u32 heading)
{
    s32 delta;
    actor->targetHeading = heading;
    delta = (actor->targetHeading - actor->heading) & 0xff;
    if (delta <= 0x80) {
        return DirectionalActor_TryBeginState(actor, 0x1a);
    }
    return DirectionalActor_TryBeginState(actor, 0x1b);
}

s32 ActorController_SetBehavior(struct SceneCombatActor *actor, s32 behavior)
{
    if (actor->behavior == behavior && actor->behaviorPhase != 0xff)
        return 0;
    actor->behavior = behavior;
    actor->behaviorPhase = 0;
    actor->behaviorCounter = 0;
    return 1;
}

u32 ActorController_SetBehaviorIfFinished(struct SceneCombatActor *actor, u32 behavior)
{
    u8 *currentBehavior = &actor->behavior;
    if (*currentBehavior != behavior) {
        u8 *phase = &actor->behaviorPhase;
        if (*phase == 0xff) {
            u32 initialPhase = 0;
            *currentBehavior = behavior;
            *phase = initialPhase;
            actor->behaviorCounter = initialPhase;
            return 1;
        }
    }
    return 0;
}

static inline void ActorController_SubtractPositions(struct ContactPosition *out,
                                                     struct ContactPosition *a,
                                                     struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

u32 ActorController_IsNearTarget(struct SceneCombatActor *actor)
{
    struct ContactPosition delta;
    ActorController_SubtractPositions(&delta, &actor->navigation.position, &actor->navigation.referencePosition);
    if (delta.x < 0)
        delta.x = -delta.x;
    if (delta.z < 0)
        delta.z = -delta.z;
    return delta.x + delta.z < 0xc0;
}

void ActorController_RequestIdleAction(struct SceneCombatActor *actor)
{
    DirectionalActor_TryBeginState(actor, 0);
}

void ActorController_UpdateTargetMovement(struct SceneCombatActor *actor, s32 movementMode, s32 attackMode)
{
    struct ContactPosition targetPosition;
    u32 result;
    u8 *phase;
    if ((actor->behaviorCounter & 7) == 0) {
        struct GameState *game = gGameState;
        targetPosition.x = game->positionX;
        targetPosition.y = game->positionY;
        targetPosition.z = game->positionZ;
        result = Terrain_ApplyBlocked(&actor->navigation, &actor->navigation.referencePosition, &targetPosition);
        if (result == 1) {
            actor->behaviorPhase = result;
        } else if (ActorController_IsNearTarget(actor) == 0) {
            actor->behaviorPhase = 1;
        }
    }
    phase = &actor->behaviorPhase;
    if (*phase == 0) {
        DirectionalActor_TryBeginState(actor, 0);
    } else if (*phase == 1) {
        u32 targetHeading = NavMesh_SteerCardinal(&actor->navigation, &actor->navigation.referencePosition, actor->heading, 0x40);
        if (ActorController_IsNearTarget(actor) != 0) {
            *phase = 0;
            DirectionalActor_TryBeginState(actor, 0);
        } else if (actor->actionReady != 0) {
            if (NavMesh_ConsumeProbeEvent(actor->navigation.probe) != 0) {
                switch (movementMode) {
                    case 0:
                        ActorController_StartTurn(actor, targetHeading);
                        break;
                    case 1:
                        ActorController_StartModeOneTurn(actor, targetHeading);
                        break;
                    case 2:
                        ActorController_StartModeTwoTurn(actor, targetHeading);
                        break;
                    case 3:
                        ActorController_StartModeThreeTurn(actor, targetHeading);
                        break;
                }
            } else {
                switch (attackMode) {
                    case 1:
                        DirectionalActor_TryBeginState(actor, 0x1f);
                        break;
                    case 2:
                        DirectionalActor_TryBeginState(actor, 0x2c);
                        break;
                    case 0:
                        switch (movementMode) {
                            case 0:
                                DirectionalActor_TryBeginState(actor, 1);
                                break;
                            case 1:
                                DirectionalActor_TryBeginState(actor, 0x13);
                                break;
                            case 2:
                                DirectionalActor_TryBeginState(actor, 0x16);
                                break;
                            case 3:
                                DirectionalActor_TryBeginState(actor, 0x26);
                                break;
                        }
                        break;
                }
            }
        }
    }
}

void ActorController_UpdateModeZeroMovement(struct SceneCombatActor *actor)
{
    ActorController_UpdateTargetMovement(actor, 0, 0);
}

void ActorController_UpdateModeTwoMovement(struct SceneCombatActor *actor)
{
    ActorController_UpdateTargetMovement(actor, 2, 0);
}

void ActorController_UpdateModeThreeMovement(struct SceneCombatActor *actor)
{
    ActorController_UpdateTargetMovement(actor, 3, 0);
}

void ActorController_UpdateModeOneMovement(struct SceneCombatActor *actor)
{
    ActorController_UpdateTargetMovement(actor, 1, 0);
}
