#include "registry_actor.h"

s32 ActorMotion_UpdateDampedFall(struct RegistryActor *actor)
{
    struct RegistryActor *motion = actor;
    s32 result = MotionTween_Advance(actor, &actor->control.motion);
    if (!actor->stats.grounded && !result) {
        actor->stats.motion.forward -= 32;
        actor->stats.song = 0x87;
    }
    motion->stats.motion.lateral = Div(motion->stats.motion.lateral * 9, 10);
    Character_UpdateLoopingSound(motion);
    if (!result && !actor->control.motion.remaining)
        return 1;
    return 0;
}

void ActorMotion_UpdateFallFrame(struct RegistryActor *actor)
{
    struct RegistryActor *motion = actor;
    s32 forwardSpeed;
    s32 displacementX, displacementZ;
    u32 verticalNonnegative, horizontalNonnegative;
    s32 forwardLimit;
    s32 *previousOffset, *currentOffset, *verticalPosition, *lateralPosition, *previousZ;

    if (actor->resetKeys) {
        RegistryA8CB_SetKeyframes(actor, 1);
        actor->resetKeys = 0;
    }
    {
        u16 bit = 0x10;
        actor->control.updateFlags |= bit;
    }
    ActorMotion_UpdateDampedFall(actor);
    forwardSpeed = actor->stats.motion.forward + actor->stats.boost;
    actor->stats.motion.vertical += 0x200;
    if (actor->stats.knockbackTimer > 0) {
        actor->stats.movementOffset += actor->stats.knockback.forward;
        actor->stats.knockback.forward += Div(-actor->stats.knockback.forward, actor->stats.knockbackTimer);
        actor->stats.knockback.vertical += Div(-actor->stats.knockback.vertical, actor->stats.knockbackTimer);
        actor->stats.knockback.lateral += Div(-actor->stats.knockback.lateral, actor->stats.knockbackTimer);
        actor->stats.knockbackTimer--;
    } else {
        Memory_Clear(&actor->stats.knockback.forward, 16);
    }
    motion->stats.position.stepX = forwardSpeed + motion->stats.movementOffset - motion->stats.relativeOrigin[0];
    motion->stats.position.stepY = motion->stats.motion.vertical + motion->stats.knockback.vertical;
    motion->stats.position.stepZ = motion->stats.motion.lateral + motion->stats.knockback.lateral;
    displacementX = (motion->stats.movementOffset - motion->stats.relativeOrigin[0]) >> 12;
    displacementZ = (motion->stats.position.z - motion->stats.relativeOrigin[2]) >> 12;
    forwardLimit = 0x3000;
    currentOffset = &motion->stats.movementOffset;
    previousOffset = &motion->stats.relativeOrigin[0];
    verticalPosition = &motion->stats.motion.vertical;
    lateralPosition = &motion->stats.motion.lateral;
    previousZ = &motion->stats.relativeOrigin[2];
    if (forwardSpeed > forwardLimit)
        displacementX = 0x80;
    if (displacementX == 0 && displacementZ == 0)
        motion->directionAngle = 0xFFFF;
    else
        motion->directionAngle = (Math_Atan2(displacementX, displacementZ) >> 5) << 5;
    *previousOffset = *currentOffset;
    *previousZ = motion->stats.position.z;
    Terrain_ResolveActorMotion(motion);
    verticalNonnegative = (u32) ~(motion->stats.verticalClearance >> 12) >> 31;
    horizontalNonnegative = (u32) ~(motion->stats.height >> 12) >> 31;
    if (motion->stats.terrainState != motion->stats.previousTerrainState && (motion->stats.terrainState & 0x1000) && !(motion->stats.previousTerrainState & 0xF000) &&
        horizontalNonnegative <= 1 && verticalNonnegative == 1) {
        s32 v = ((motion->stats.motion.forward >> 10) + (motion->stats.boost >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 18)
            v = 18;
        *verticalPosition -= v << 9;
        motion->stats.motionFlags &= ~1;
    }
    actor->control.lastForwardSpeed = forwardSpeed;
    PalFade_Update(motion);
    Actor_UpdateShadowSpriteParts(motion, &actor->model);
    if (motion->stats.motionFlags & 4)
        ActorParticles_SpawnImpactBurst(motion);
    if (motion->stats.motionFlags & 2)
        *lateralPosition = 0;
    if (motion->stats.motionFlags & 1)
        *verticalPosition = 0;
    CharacterModel_UpdateAccessoryAnimations(&actor->model);
    actor->fallTimer++;
}

void SceneActor_UpdateFallState(struct RegistryActor *actor)
{
    struct RegistryActor *motion = actor;
    s32 forwardSpeed;
    s32 dx, dz;
    u32 verticalNonnegative, horizontalNonnegative;
    s32 forwardLimit;
    s32 *prevX, *curX, *posY, *posZ, *prevZ;

    if (actor->resetKeys) {
        RegistryA8CB_SetKeyframes(actor, 1);
        Trigger_ConsumePending(actor->control.messages, 1);
        actor->resetKeys = 0;
    }
    {
        u16 bit = 0x10;
        actor->control.updateFlags |= bit;
    }
    ActorMotion_UpdateDampedFall(actor);
    forwardSpeed = actor->stats.motion.forward + actor->stats.boost;
    actor->stats.motion.vertical += 0x200;
    if (actor->stats.knockbackTimer > 0) {
        actor->stats.movementOffset += actor->stats.knockback.forward;
        actor->stats.knockback.forward += Div(-actor->stats.knockback.forward, actor->stats.knockbackTimer);
        actor->stats.knockback.vertical += Div(-actor->stats.knockback.vertical, actor->stats.knockbackTimer);
        actor->stats.knockback.lateral += Div(-actor->stats.knockback.lateral, actor->stats.knockbackTimer);
        actor->stats.knockbackTimer--;
    } else {
        Memory_Clear(&actor->stats.knockback.forward, 0x10);
    }
    motion->stats.position.stepX = forwardSpeed + motion->stats.movementOffset - motion->stats.relativeOrigin[0];
    motion->stats.position.stepY = motion->stats.motion.vertical + motion->stats.knockback.vertical;
    motion->stats.position.stepZ = motion->stats.motion.lateral + motion->stats.knockback.lateral;
    dx = (motion->stats.movementOffset - motion->stats.relativeOrigin[0]) >> 12;
    dz = (motion->stats.position.z - motion->stats.relativeOrigin[2]) >> 12;
    forwardLimit = 0x3000;
    curX = &motion->stats.movementOffset;
    prevX = &motion->stats.relativeOrigin[0];
    posY = &motion->stats.motion.vertical;
    posZ = &motion->stats.motion.lateral;
    prevZ = &motion->stats.relativeOrigin[2];
    if (forwardSpeed > forwardLimit)
        dx = 0x80;
    if (dx == 0 && dz == 0)
        motion->directionAngle = 0xFFFF;
    else
        motion->directionAngle = Math_Atan2(dx, dz) >> 5 << 5;
    *prevX = *curX;
    *prevZ = motion->stats.position.z;
    Terrain_ResolveActorMotion(motion);
    verticalNonnegative = (u32) ~(motion->stats.verticalClearance >> 12) >> 31;
    horizontalNonnegative = (u32) ~(motion->stats.height >> 12) >> 31;
    if (motion->stats.terrainState != motion->stats.previousTerrainState && (motion->stats.terrainState & 0x1000) && !(motion->stats.previousTerrainState & 0xF000) && horizontalNonnegative <= 1 &&
        verticalNonnegative == 1) {
        s32 v = ((motion->stats.motion.forward >> 10) + (motion->stats.boost >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 18)
            v = 18;
        *posY -= v << 9;
        motion->stats.motionFlags &= ~1;
    }
    actor->control.lastForwardSpeed = forwardSpeed;
    PalFade_Update(motion);
    Actor_UpdateShadowSpriteParts(motion, &actor->model);
    if (motion->stats.motionFlags & 4)
        ActorParticles_SpawnImpactBurst(motion);
    if (motion->stats.motionFlags & 2)
        *posZ = 0;
    if (motion->stats.motionFlags & 1)
        *posY = 0;
    CharacterModel_UpdateAccessoryAnimations(&actor->model);
    actor->fallTimer++;
}

static inline void RegistryActor_BeginMotion(struct RegistryActor *actor, void (*fn)(struct RegistryActor *))
{
    actor->resetKeys = 1;
    actor->control.update = fn;
    actor->fallTimer = 0;
}

void SceneActor_UpdateKnockbackFall(struct RegistryActor *actor)
{
    struct RegistryActor *motion = actor;
    u8 finished;
    s32 forwardSpeed;
    s32 dx, dz;
    s32 verticalNonnegative, horizontalNonnegative;
    s32 forwardLimit;
    s32 *prev, *cur, *py, *pz, *prevZ;

    if (actor->resetKeys) {
        RegistryA8CB_SetKeyframes(actor, 1);
        actor->resetKeys = 0;
    }
    {
        u16 bit = 0x10;
        actor->control.updateFlags |= bit;
    }
    finished = ActorMotion_UpdateDampedFall(actor) != 0;
    forwardSpeed = actor->stats.motion.forward + actor->stats.boost;
    actor->stats.motion.vertical += 0x200;
    if (actor->stats.knockbackTimer > 0) {
        actor->stats.movementOffset += actor->stats.knockback.forward;
        actor->stats.knockback.forward += Div(-actor->stats.knockback.forward, actor->stats.knockbackTimer);
        actor->stats.knockback.vertical += Div(-actor->stats.knockback.vertical, actor->stats.knockbackTimer);
        actor->stats.knockback.lateral += Div(-actor->stats.knockback.lateral, actor->stats.knockbackTimer);
        actor->stats.knockbackTimer--;
    } else {
        Memory_Clear(&actor->stats.knockback, 0x10);
    }
    motion->stats.position.stepX = forwardSpeed + motion->stats.movementOffset - motion->stats.relativeOrigin[0];
    motion->stats.position.stepY = motion->stats.motion.vertical + motion->stats.knockback.vertical;
    motion->stats.position.stepZ = motion->stats.motion.lateral + motion->stats.knockback.lateral;
    dx = (motion->stats.movementOffset - motion->stats.relativeOrigin[0]) >> 12;
    dz = (motion->stats.position.z - motion->stats.relativeOrigin[2]) >> 12;
    forwardLimit = 0x3000;
    cur = &motion->stats.movementOffset;
    prev = &motion->stats.relativeOrigin[0];
    py = &motion->stats.motion.vertical;
    pz = &motion->stats.motion.lateral;
    prevZ = &motion->stats.relativeOrigin[2];
    if (forwardSpeed > forwardLimit)
        dx = 0x80;
    if (dx == 0 && dz == 0)
        motion->directionAngle = 0xFFFF;
    else
        motion->directionAngle = (Math_Atan2(dx, dz) >> 5) << 5;
    *prev = *cur;
    *prevZ = motion->stats.position.z;
    Terrain_ResolveActorMotion(motion);
    verticalNonnegative = (u32) ~(motion->stats.verticalClearance >> 12) >> 31;
    horizontalNonnegative = (u32) ~(motion->stats.height >> 12) >> 31;
    if (motion->stats.terrainState != motion->stats.previousTerrainState && (motion->stats.terrainState & 0x1000) && !(motion->stats.previousTerrainState & 0xF000) &&
        (u32)horizontalNonnegative <= 1 && verticalNonnegative == 1) {
        s32 t = ((motion->stats.motion.forward >> 10) + (motion->stats.boost >> 8)) >> 1;
        if (t < 4)
            t = 4;
        if (t > 18)
            t = 18;
        *py -= t << 9;
        motion->stats.motionFlags &= ~1;
    }
    actor->control.lastForwardSpeed = forwardSpeed;
    Actor_UpdateShadowSpriteParts(motion, &actor->model);
    if (motion->stats.motionFlags & 4)
        ActorParticles_SpawnImpactBurst(motion);
    if (motion->stats.motionFlags & 2)
        *pz = 0;
    if (motion->stats.motionFlags & 1)
        *py = 0;
    CharacterModel_UpdateAccessoryAnimations(&actor->model);
    if (finished == 1) {
        Trigger_ConsumePending(actor->control.messages, 1);
        RegistryActor_BeginMotion(actor, ActorPhysics_UpdateFallMotion);
    } else {
        actor->fallTimer++;
    }
}

void ActorPhysics_UpdateFallMotion(struct RegistryActor *actor)
{
    struct RegistryActor *motion = actor;
    s32 forwardSpeed;
    s32 dx, dz;
    u32 verticalNonnegative, horizontalNonnegative;

    if (actor->resetKeys) {
        RegistryA8CB_SetKeyframes(actor, 1);
        actor->resetKeys = 0;
    }
    {
        u16 bit = 0x10;
        actor->control.updateFlags |= bit;
    }
    ActorMotion_UpdateDampedFall(actor);
    forwardSpeed = actor->stats.motion.forward + actor->stats.boost;
    actor->stats.motion.vertical += 0x200;
    if (actor->stats.knockbackTimer > 0) {
        actor->stats.movementOffset += actor->stats.knockback.forward;
        actor->stats.knockback.forward += Div(-actor->stats.knockback.forward, actor->stats.knockbackTimer);
        actor->stats.knockback.vertical += Div(-actor->stats.knockback.vertical, actor->stats.knockbackTimer);
        actor->stats.knockback.lateral += Div(-actor->stats.knockback.lateral, actor->stats.knockbackTimer);
        actor->stats.knockbackTimer--;
    } else {
        Memory_Clear(&actor->stats.knockback.forward, 0x10);
    }
    {
        s32 *pprevZ, *pprevX, *pvz, *px, *py;
        s32 forwardLimit;
        motion->stats.position.stepX = forwardSpeed + motion->stats.movementOffset - motion->stats.relativeOrigin[0];
        motion->stats.position.stepY = motion->stats.motion.vertical + motion->stats.knockback.vertical;
        motion->stats.position.stepZ = motion->stats.motion.lateral + motion->stats.knockback.lateral;
        dx = (motion->stats.movementOffset - motion->stats.relativeOrigin[0]) >> 12;
        dz = (motion->stats.position.z - motion->stats.relativeOrigin[2]) >> 12;
        forwardLimit = 0x3000;
        px = &motion->stats.movementOffset;
        pprevX = &motion->stats.relativeOrigin[0];
        py = &motion->stats.motion.vertical;
        pprevZ = &motion->stats.relativeOrigin[2];
        pvz = &motion->stats.motion.lateral;
        if (forwardSpeed > forwardLimit)
            dx = 0x80;
        if (dx == 0 && dz == 0)
            motion->directionAngle = 0xFFFF;
        else
            motion->directionAngle = (Math_Atan2(dx, dz) >> 5) << 5;
        *pprevX = *px;
        *pprevZ = motion->stats.position.z;
        Terrain_ResolveActorMotion(motion);
        verticalNonnegative = (u32) ~(motion->stats.verticalClearance >> 12) >> 31;
        horizontalNonnegative = (u32) ~(motion->stats.height >> 12) >> 31;
        if (motion->stats.terrainState != motion->stats.previousTerrainState && (motion->stats.terrainState & 0x1000) && !(motion->stats.previousTerrainState & 0xF000) && horizontalNonnegative <= 1 &&
            verticalNonnegative == 1) {
            s32 v = ((motion->stats.motion.forward >> 10) + (motion->stats.boost >> 8)) >> 1;
            if (v < 4)
                v = 4;
            if (v > 18)
                v = 18;
            *py -= v << 9;
            motion->stats.motionFlags &= ~1;
        }
        actor->control.lastForwardSpeed = forwardSpeed;
        Actor_UpdateShadowSpriteParts(motion, &actor->model);
        if (motion->stats.motionFlags & 4)
            ActorParticles_SpawnImpactBurst(motion);
        if (motion->stats.motionFlags & 2)
            *pvz = 0;
        if (motion->stats.motionFlags & 1)
            *py = 0;
        CharacterModel_UpdateAccessoryAnimations(&actor->model);
        actor->fallTimer++;
    }
}
