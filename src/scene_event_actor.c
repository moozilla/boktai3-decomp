#include "scene_event_actor.h"

static inline u16 RandomSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}

void SceneEventActor_UpdateJitter(struct ActorState *eventActor)
{
    if (eventActor->changed) {
        eventActor->changed = 0;
        SceneActors_StartDirectionalAnimation(eventActor, &eventActor->graphics.id, eventActor->animationData, 0x114, 1, 1, 4);
        eventActor->jitterTicks = 6;
    }
    if (eventActor->jitterTicks) {
        s32 random = RandomSample();
        eventActor->randomOffset = random >> 8;
        eventActor->jitterTicks--;
        {
            u32 flags = 0x20000;
            eventActor->flags |= flags;
        }
    }
    {
        u32 flags = 0x400005;
        eventActor->flags |= flags;
    }
    eventActor->flags |= 0x40;
    Callback_InvokeActor(eventActor, eventActor->callback);
}

static inline s32 ScaleMovement(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

void SceneEventActor_UpdateRecoil(struct ActorState *eventActor)
{
    struct ActorState *actor = eventActor;
    if (eventActor->changed) {
        eventActor->changed = 0;
        SceneActors_StartDirectionalAnimation(eventActor, &eventActor->graphics.id, eventActor->animationData, 0x11a, 1, 1, 4);
        eventActor->flags |= 1;
    }
    switch (eventActor->stateReset) {
        case 0:
            if (eventActor->hitParameter) {
                u32 angle = eventActor->direction;
                eventActor->body.motion.x = ScaleMovement(gUnk_086149C4[(angle + 64) & 255] * eventActor->hitParameter);
                eventActor->body.motion.z = ScaleMovement(gUnk_086149C4[angle] * actor->hitParameter);
                actor->hitParameter = (actor->hitParameter * 3) >> 3;
                {
                    u32 one = 1;
                    eventActor->flags |= one;
                }
            } else {
                eventActor->hitParameter = 0;
                SceneActors_StartDirectionalAnimation(eventActor, &eventActor->graphics.id, eventActor->animationData, 0x11c, 1,
                                                      1, 1);
                if (eventActor->finished)
                    SceneActors_StartDirectionalAnimation(eventActor, &eventActor->graphics.id, eventActor->animationData, 0x11e,
                                                          1, 1, 1);
            }
            /* Fall through while the movement animation settles. */
        case 1:
            ++eventActor->pendingAction;
            if (eventActor->finished || eventActor->pendingAction > 30) {
                SceneActors_StartEightDirectionAnimation(eventActor, &actor->graphics.id, actor->animationData,
                                                         0xf4, 1, 1, 1);
                {
                    u32 next = 2;
                    u32 zero = 0;
                    eventActor->stateReset = next;
                    eventActor->pendingAction = zero;
                }
            }
            break;
        case 2:
            eventActor->pending = 1;
            break;
    }
}

void SceneEventActor_UpdateRemovalSequence(struct ActorState *actor)
{
    if (actor->changed) {
        actor->changed = 0;
        actor->stateReset = 0;
        actor->pendingAction = 0;
        SceneActors_StartEightDirectionAnimation(actor, &actor->graphics.id, actor->animationData, 0x114, 2,
                                                 1, 4);
        Sound_PlaySong(0x2EB);
        SceneSteeringActor_RequestRemovalAll();
    }
    switch (actor->stateReset) {
        case 0:
            if (++actor->pendingAction > 15 && !Player_IsSlotInActionTwelveOrThirtyEight(0)) {
                SceneActors_RunFirstEventScript(actor, actor->graphics.eventScripts);
                actor->stateReset = 1;
                actor->pendingAction = 0;
            }
            break;
        case 1:
            if (!actor->pendingAction) {
                SceneActors_StartEightDirectionAnimation(actor, &actor->graphics.id, actor->animationData,
                                                         0x148, 2, 1, 4);
                Anim_SetFrameDurationScale(&actor->graphics.id, 0x100);
                actor->pendingAction++;
            }
            if (actor->finished) {
                {
                    u32 phase = 2;
                    u32 zero = 0;
                    actor->stateReset = phase;
                    actor->pendingAction = zero;
                }
            }
            break;
        case 2:
            if (actor->graphics.eventFlags & 2) {
                {
                    u32 phase = 3;
                    u32 zero = 0;
                    actor->stateReset = phase;
                    actor->pendingAction = zero;
                }
                SceneActors_RunSecondEventScript(actor, actor->graphics.eventScripts);
            }
            break;
    }
    {
        u32 mask = 0xD;
        actor->flags |= mask;
    }
}

static inline void SceneEventActor_SetDirection(struct ActorState *eventActor, int angle)
{
    eventActor->body.direction = angle;
    {
        s32 d = (eventActor->body.direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= d;
        eventActor->facing4 = mask;
    }
    {
        s32 d = (eventActor->body.direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= d;
        eventActor->facing8 = mask;
    }
}

static inline s32 ScaleCoordinate(s32 value)
{
    return value >= 0 ? (s16)(value >> 12) : -(s16)((-value) >> 12);
}

static inline int TerrainHeight(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    u32 cell;
    u8 *data;
    u32 shape, flags, height;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    flags = *data;
    shape = flags >> 4;
    {
        u32 mask = 15;
        mask &= flags;
        height = mask << 8;
    }
    switch (shape) {
        case 1:
            height -= *(u8 *)&position->z;
            break;
        case 2:
            height -= *(u8 *)&position->x;
            break;
    }
    return height;
}

static inline void SetVelocity(struct ContactPosition *velocity, u32 angle, s32 speed)
{
    s32 x = ScaleCoordinate(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    velocity->x = x;
    velocity->y = zero;
    velocity->z = ScaleCoordinate(gUnk_086149C4[angle & 255] * speed);
}

static inline void SceneEventActor_SetPalette(struct ActorState *eventActor, u32 mode, u32 shift)
{
    eventActor->paletteMode = mode;
    eventActor->paletteShift = shift;
    eventActor->paletteTimer = 0;
    eventActor->palette = eventActor->paletteData;
}

static inline void AddPosition(struct ContactPosition *out, struct SceneActorBody *a, struct ContactPosition *b)
{
    out->x = b->x + a->position.x;
    out->y = a->position.y + b->y;
    out->z = b->z + a->position.z;
}

static inline void SetHorizontalVelocity(struct ActorState *eventActor, u32 angle, s32 speed)
{
    eventActor->body.motion.x = ScaleCoordinate(gUnk_086149C4[(angle + 64) & 255] * speed);
    eventActor->body.motion.z = ScaleCoordinate(gUnk_086149C4[angle & 255] * speed);
}

static inline void FinishLunge(struct ActorState *eventActor, void *callback, u32 one)
{
    eventActor->controlPhase = one;
    eventActor->stateCallback = callback;
    eventActor->stateTicks = 0;
    eventActor->changed = one;
    eventActor->stateReset = 0;
    eventActor->pendingAction = 0;
    eventActor->pending = one;
}

static inline int HeightInvalid(u32 difference, u32 limit)
{
    return difference > limit;
}

void BossSlot2_UpdateLungeApproach(struct ActorState *eventActor)
{
    int zero = 0;
    struct ActorState *actor = eventActor;
    int dx, dz, angle;
    int speed;
    if (eventActor->changed) {
        eventActor->changed = zero;
        ProximityVolume_TranslateBounds(eventActor->bounds, &eventActor->body.position);
        actor->animationPhase = 0;
        if (actor->attackFlags & 2)
            Sound_PlaySong(0x540);
        actor->attackFlags &= ~2;
    }
    if (actor->animationState == 1)
        SceneVolume_TestPlayerPosition(actor);
    else if (actor->animationState == 3)
        ActorPosition_TestPath(actor, &actor->animation);
    {
        u32 mask = 0x480004;
        eventActor->flags |= mask;
    }
    if (eventActor->pendingAction == 8)
        Sound_PlaySong(0x344);
    if (++eventActor->pendingAction > 15) {
        u32 mask = ~0x50010;
        eventActor->flags &= mask;
        FinishLunge(eventActor, ActorAnimation_UpdateDirectional, 1);
        return;
    }
    if ((u32)(eventActor->pendingAction - 5) <= 7) {
        u32 mask = 0x10010;
        eventActor->flags |= mask;
    } else {
        u32 mask = 0x50000;
        eventActor->flags |= mask;
    }
    if (eventActor->pendingAction > 7)
        SceneEventActor_SetPalette(actor, 1, 5);
    {
        int radiusSquared = 2500;
        speed = 32;
        dx = actor->destination.x - eventActor->body.position.x;
        dz = actor->destination.z - eventActor->body.position.z;
        angle = Math_Atan2(dx, dz) & 0xE0;
        if (dx * dx + dz * dz > radiusSquared) {
            struct SceneActorBody *base = &actor->body;
            struct ContactPosition position;
            int invalid;
            u32 heightDifference;
            SetVelocity(&position, angle, speed);
            AddPosition(&position, base, &position);
            heightDifference = TerrainHeight(&position) - actor->floorHeight + 511;
            invalid = HeightInvalid(heightDifference, 766);
            if (!invalid)
                goto advance;
        }
        eventActor->body.motion.x = dx;
        eventActor->body.motion.z = dz;
        actor->animationPhase = 1;
        SceneActors_StartEightDirectionAnimation(eventActor, &actor->graphics.id, actor->animationData, 0xF4,
                                                 1, 1, 4);
        return;
    }
advance:
    SetHorizontalVelocity(eventActor, angle, speed);
    SceneEventActor_SetDirection(eventActor, angle);
    SceneActors_StartEightDirectionAnimation(eventActor, &actor->graphics.id, actor->animationData, 0xF9, 1,
                                             1, 4);
}

void SceneEventActor_UpdateTimedAnimation(struct ActorState *actor, s32 tick)
{
    if (actor->changed) {
        actor->changed = 0;
        actor->body.direction = actor->requestedDirection;
        {
            int shifted;
            u32 value;
            shifted = (actor->body.direction + 0x60) >> 6;
            value = 3;
            value &= shifted;
            actor->facing4 = value;
            shifted = (actor->body.direction + 0x70) >> 5;
            value = 7;
            value &= shifted;
            actor->facing8 = value;
        }
        {
            u32 mask = 0x400004;
            actor->flags |= mask;
        }
    }
    SceneActors_StartEightDirectionAnimation(actor, &actor->graphics.id, actor->animationData, 0xF4,
                                             1, 1, 4);
    if (tick == 1) {
        if (actor->variant == 6)
            ActorAttachment_PlayEffect(actor, 3);
        else
            ActorAttachment_PlayEffect(actor, 1);
    }
    if (tick > 30)
        actor->pending = 1;
    if (!(u8)Callback_InvokeActor(actor, actor->callback)) {
        u32 mask = 0x40;
        actor->flags |= mask;
    }
}

void SceneEventActor_UpdateTimedResponse(struct ActorState *actor, s32 tick)
{
    if (actor->changed) {
        actor->changed = 0;
        ActorAttachment_PlayEffect(actor, 2);
        SceneActors_StartEightDirectionAnimation(actor, &actor->graphics.id, actor->animationData,
                                                 0xF4, 1, 1, 4);
        {
            u32 mask = 0x400004;
            actor->flags |= mask;
        }
    }
    if (tick > 30)
        actor->pending = 1;
    if (!(u8)Callback_InvokeActor(actor, actor->callback)) {
        u32 mask = 0x40;
        actor->flags |= mask;
    }
}

static inline u16 FourWay(u16 angle)
{
    return ((angle + 0x60) >> 6) & 3;
}

static inline u16 EightWay(u16 angle)
{
    return ((angle + 0x70) >> 5) & 7;
}

static inline void BossSlot2_SetDirection(struct ActorState *eventActor, u32 heading)
{
    eventActor->body.direction = heading;
    eventActor->facing4 = FourWay(eventActor->body.direction);
    eventActor->facing8 = EightWay(eventActor->body.direction);
}

void BossSlot2_UpdateSummonSpinSequence(struct ActorState *eventActor)
{
    struct ActorState *boss = eventActor;
    if (eventActor->changed) {
        eventActor->changed = 0;
        boss->impactCount = 0;
        eventActor->stateReset = 0;
        eventActor->pendingAction = 0;
    }
    {
        u32 mask = 0x400004;
        eventActor->flags |= mask;
    }
    switch (eventActor->stateReset) {
        case 0: {
            u32 zero = eventActor->pendingAction;
            if (!zero) {
                BossSlot2_SetDirection(eventActor, 32);
                {
                    u32 paletteMode = 3;
                    SceneActors_StartEightDirectionAnimation(eventActor, &boss->graphics.id, boss->animationData,
                                                             0xF4, 1, 1, 4);
                    Sound_PlaySong(0x2E4);
                    Sound_PlaySong(0x1A8);
                    {
                        u8 paletteShift = 5;
                        boss->paletteMode = paletteMode;
                        boss->paletteShift = paletteShift;
                    }
                    boss->paletteTimer = zero;
                    boss->palette = boss->paletteData;
                }
            }
            if ((++eventActor->pendingAction) > 8) {
                u32 phase = 1;
                u32 zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            }
            break;
        }

        case 1:
            if (!eventActor->pendingAction) {
                SceneActors_StartUnmirroredAnimation(eventActor, &boss->graphics.id, boss->animationData, 0x12B, 2,
                                                     4);
                eventActor->pendingAction++;
            }
            if (eventActor->finished == 1) {
                u32 phase = 2;
                u32 zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            }
            break;

        case 2:
            if (!eventActor->pendingAction) {
                SceneActors_StartUnmirroredAnimation(eventActor, &boss->graphics.id, boss->animationData, 0x12D, 2,
                                                     4);
                BossSlot2_SpawnBobbingFormation(eventActor);
            }
            if (eventActor->pendingAction > 30) {
                u32 phase = 3;
                u32 zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            } else
                eventActor->pendingAction++;
            break;

        case 3:
            if (!eventActor->pendingAction)
                SceneActors_StartUnmirroredAnimation(eventActor, &boss->graphics.id, boss->animationData, 0x12F, 2,
                                                     4);
            if ((++eventActor->pendingAction) > 30) {
                u32 phase = 4;
                u32 zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            }
            break;

        case 4:
            if (!eventActor->pendingAction) {
                BossSlot2_SetDirection(eventActor, 0);
                SceneActors_StartEightDirectionAnimation(eventActor, &boss->graphics.id, boss->animationData, 0x134,
                                                         1, 1, 4);
                Sound_PlaySong(0x2E9);
            }
            if ((++eventActor->pendingAction) > 20) {
                u32 phase = 5;
                u32 zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            }
            break;

        case 5: {
            BossSlot2_SetDirection(eventActor, eventActor->pendingAction + 15);
            SceneActors_StartEightDirectionAnimation(eventActor, &boss->graphics.id, boss->animationData, 0x13E, 1,
                                                     1, 1);
            ActorEffect_SpawnAtAngle(boss->effect, &eventActor->body.position, eventActor->pendingAction);
            {
                u32 mask = 0x100000;
                eventActor->flags |= mask;
                eventActor->flags &= ~0x400000;
            }
            if ((eventActor->pendingAction += 16) > 512) {
                eventActor->stateReset = 7;
                eventActor->pendingAction = 0;
            }
            if (eventActor->pendingAction == 128)
                sub_0806CCC8(eventActor->selectedPoint, 180, -2);
        } break;

        case 7:
            if (!eventActor->pendingAction) {
                BossSlot2_SetDirection(eventActor, 0);
                SceneActors_StartEightDirectionAnimation(eventActor, &boss->graphics.id, boss->animationData, 0x134,
                                                         1, 1, 4);
            }
            if (eventActor->pendingAction > 20) {
                u32 phase = 8;
                u32 zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            } else
                eventActor->pendingAction++;
            break;

        case 8:
            if (!eventActor->pendingAction) {
                BossSlot2_SetDirection(eventActor, 32);
                SceneActors_StartEightDirectionAnimation(eventActor, &boss->graphics.id, boss->animationData, 0xF4,
                                                         1, 1, 1);
                eventActor->pendingAction++;
            }
            eventActor->pending = 1;
            {
                u32 mask = ~4;
                eventActor->flags &= mask;
            }
            Callback_InvokeActor(eventActor, eventActor->callback);
            break;
    }
}

void SceneEventActor_UpdateDelayedFinish(struct ActorState *actor)
{
    if (actor->changed)
        actor->changed = 0;
    switch (actor->stateReset) {
        case 0:
            if (!actor->pendingAction) {
                SceneActors_StartEightDirectionAnimation(actor, &actor->graphics.id, actor->animationData,
                                                         0x148, 2, 1, 1);
                actor->pendingAction++;
            }
            if (actor->finished) {
                ProximityVolume_SetPosition(actor->proximityVolume, &actor->body.position);
                actor->stateReset = 1;
                actor->pendingAction = 0;
            }
            break;
        case 1:
            if (!actor->pendingAction)
                SceneActors_StartEightDirectionAnimation(actor, &actor->graphics.id, actor->animationData,
                                                         0x14D, 1, 1, 1);
            {
                u32 mask = 0x200000;
                actor->flags |= mask;
            }
            actor->pendingAction++;
            {
                u32 mask = 0x50000;
                actor->flags |= mask;
            }
            if (actor->pendingAction > 120) {
                ProximityVolume_SetPosition(actor->proximityVolume, &actor->body.position);
                {
                    u32 phase = 2;
                    u32 zero = 0;
                    actor->stateReset = phase;
                    actor->pendingAction = zero;
                }
            }
            break;
        case 2: {
            if (gGameState->unk876 <= 0) {
                u32 mask = 0x10010;
                actor->flags |= mask;
            } else {
                u32 mask = 0x50000;
                actor->flags |= mask;
            }
            actor->pending = 1;
        } break;
    }
    if (!(u8)Callback_InvokeActor(actor, actor->callback)) {
        u32 mask = 0x400000;
        actor->flags |= mask;
    }
}

static inline void BossSlot2_SetChargePalette(struct ActorState *eventActor, u32 mode, u32 shift, u32 zero)
{
    eventActor->paletteMode = mode;
    eventActor->paletteShift = shift;
    eventActor->paletteTimer = zero;
    eventActor->palette = eventActor->paletteData;
}

void BossSlot2_UpdateChargePaletteSequence(struct ActorState *eventActor)
{
    if (eventActor->changed) {
        eventActor->changed = 0;
        SceneEventActor_SetDirection(eventActor, 32);
        SceneActors_StartEightDirectionAnimation(eventActor, &eventActor->graphics.id, eventActor->animationData, 0xF4, 1, 1,
                                                 1);
        ActorPosition_SetOffsetPoint(eventActor);
        eventActor->stateReset = 0;
        eventActor->pendingAction = 0;
    }
    {
        u32 mask = 0x400004;
        eventActor->flags |= mask;
    }
    switch (eventActor->stateReset) {
        case 0:
            if (!eventActor->pendingAction) {
                SceneActors_StartUnmirroredAnimation(eventActor, &eventActor->graphics.id, eventActor->animationData, 0x12B, 2,
                                                     4);
                eventActor->pendingAction++;
            }
            if (eventActor->finished == 1) {
                u32 phase = 1, zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            }
            break;
        case 1:
            if (!eventActor->pendingAction)
                SceneActors_StartUnmirroredAnimation(eventActor, &eventActor->graphics.id, eventActor->animationData, 0x12D, 2,
                                                     4);
            if (eventActor->pendingAction > 30) {
                u32 phase = 2, zero = 0;
                eventActor->stateReset = phase;
                eventActor->pendingAction = zero;
            } else
                eventActor->pendingAction++;
            break;
        case 2:
            if (!eventActor->pendingAction) {
                BossSlot2_SetChargePalette(eventActor, 4, 5, eventActor->pendingAction);
                Sound_PlaySong(0x239);
                eventActor->finished = 1;
                eventActor->pendingAction++;
            }
            SceneActors_SpawnConvergingParticle(eventActor, &eventActor->offsetPoint);
            break;
    }
}

void SceneEventActor_UpdateSongStopTransition(struct ActorState *actor)
{
    s16 k2;
    if (actor->changed) {
        actor->changed = 0;
        actor->stateReset = 0;
        actor->pendingAction = 0;
        Sound_StopSong(0x239);
    }
    {
        u32 mask = 0x400004;
        actor->flags |= mask;
    }
    switch (actor->stateReset) {
        case 0:
            if (++actor->pendingAction > 8 && !actor->paletteTimer) {
                SceneVolume_ResetPendingSequence(actor);
                actor->stateReset = 1;
                actor->pendingAction = 0;
            }
            break;
        case 2:
        default:
            break;
        case 1:
            if (!actor->pendingAction) {
                SceneActors_StartUnmirroredAnimation(actor, &actor->graphics.id,
                                                     actor->animationData, 0x12F, 2, 4);
                actor->pendingAction++;
            }
            if (actor->finished) {
                u32 zero;
                u32 phase;
                int shifted;
                u32 facing;
                facing = 0x20;
                actor->body.direction = facing;
                zero = 0;
                k2 = 2;
                phase = k2;
                shifted = actor->body.direction + 0x60;
                shifted >>= 6;
                facing = 3;
                facing &= shifted;
                actor->facing4 = facing;
                {
                    int sh;
                    u32 m;
                    sh = actor->body.direction + 0x70;
                    sh >>= 5;
                    m = 7;
                    m &= sh;
                    actor->facing8 = m;
                }
                SceneActors_StartEightDirectionAnimation(actor, &actor->graphics.id,
                                                         actor->animationData, 0xF4, 1, 1, 4);
                actor->stateReset = phase;
                actor->pendingAction = zero;
                actor->pending = 1;
            }
            break;
    }
}
