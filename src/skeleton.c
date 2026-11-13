#include "skeleton.h"

/* Resolve contact flags, animation, and pursuit routing. The normal chase
 * continuation returns zero even after installing its control. */

u8 Skeleton_ReactToContact(struct SceneActorRegistryResetView *actor)
{
    struct Skeleton *body = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;

    if (!(actor->field2D8 & 0x20000)) {
        if (actor->reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x13E);
    }
    SceneActorScript_TriggerCallOnCount((struct SceneActorRegistryActor *)actor);
    if (SceneActorChase_QueryMotionFlags(actor, 0x2000)) {
        actor->body.direction = actor->field2B6 + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 0);
        SceneActorChase_ClearMotionFlags(actor, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->field2C2 = Div(0x40, 3);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x8002)) {
        SceneActorChase_ClearMotionFlags(actor, 0x20);
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        SceneActorChase_SetControl(actor, actor->reactionTarget230, 0x12);
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x1000000);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(actor, 0x20);
    if (SceneActorChase_QueryMotionFlags(actor, 0x100))
        return 0;
    if (actor->field2B8)
        return 0;
    if (SceneActorChase_TrackingFlags(&actor->tracking, 0x10))
        return 0;
    if (SceneActorChase_QueryMotionFlags(actor, 0x80)) {
        if (SceneActorChase_BeginContactAnimation(actor))
            Sound_PlaySong(0x3A7);
        if (!actor->tracking.target)
            return 0;
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            body->pursuitTimer = 0;
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        actor->field2C2 = 0x20;
        return 1;
    }
    if (SceneActorChase_BeginContactAnimation(actor))
        Sound_PlaySong(0x3A7);
    if (!actor->tracking.target)
        return 0;
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!SceneActorChase_Arrive(actor)) {
        body->pursuitTimer = 0;
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
    }
    SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->field2C2 = 0x20;
    return 0;
}

/* Skeleton_UpdateProbeApproach: control phase that moves the actor toward its nav probe descriptor (+0x5D0): turns on a probe event, otherwise accelerates; changes state on arrival */

void Skeleton_UpdateProbeApproach(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *nav = &actor->group;
    struct Skeleton *control = actor->context;
    s32 speed;
    u32 event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SceneActorChase_SetControl(actor, Skeleton_UpdateTargetTransition, 0x3B);
        {
            u32 bit = 0x200;
            u32 *flags = &control->actionFlags;
            *flags |= bit;
        }
    }
    if (SceneActorReaction_IsRangeReady(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
            return;
        }
        if (sub_0812F3CC(actor)) {
            return;
        }
    }
    speed = Callback_InvokeActor(*(&actor), actor->animationQuery.opaque);
    actor->field2C2 = 0x20;
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *p = &actor->field2C2;
            *p = 0x40;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0) {
                *scale = 0x40;
            }
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale(
            ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
    event = NavMesh_ConsumeProbeEvent(nav->route);
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, nav->route, 0x20, 0x20, speed)) {
            Skeleton_ReturnToBehavior(actor, 16);
            SceneActorChase_SetControl(actor, actor->savedTarget, 0);
            return;
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, nav->route, 0x20, 0x20,
                                                             speed)) {
        Skeleton_SetState(actor, 3, 1, 0);
        {
            u32 bit = 0x100;
            u32 *flags = &control->actionFlags;
            *flags |= bit;
        }
        return;
    }
    actor->counter2AC++;
}

void Skeleton_UpdateEntryPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *control = actor->entryTarget224;
        SceneActorChase_SetControl(actor, control, 2);
    }
    if (SceneActorReaction_IsRangeReady(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
            return;
        if (sub_0812F3CC(actor))
            return;
    }
    if (actor->field2B0) {
        Skeleton_ReturnToBehavior(actor, 15);
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
    } else
        ++actor->counter2AC;
}

void Skeleton_UpdateEntryRecoveryPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *control = actor->entryTarget224;
        SceneActorChase_SetControl(actor, control, 2);
    }
    if (SceneActorReaction_IsRangeReady(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
            return;
        if (sub_0812F3CC(actor))
            return;
    }
    if (actor->field2B0) {
        {
            u32 state = 3;
            u32 phase = 1;
            actor->state2A8 = state;
            actor->phase2A9 = phase;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
        SceneActorChase_SetControl(actor, (void *)SceneActorApproach_UpdateRange, 60);
    } else
        ++actor->counter2AC;
}

void Skeleton_SetMovementControlPreset(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks284 = (void **)gUnk_086060B4;
}

s32 Skeleton_Update(struct SceneActorRegistryResetView *actor)
{
    u32 m;
    SceneActorReaction_TickHiddenResponse(actor);
    m = 0x2000;
    if ((actor->flags2D4 & m) || actor->field2B8 == 0) {
        Callback_InvokeActor(actor, actor->callbacks27C[actor->state2A8]);
        SceneActorControl_DispatchControl(actor);
    }
    Skeleton_UpdateActionFlags(actor);
    return 1;
}

static inline u8 sameRegion(u16 *region)
{
    if (gGameState->region == *region)
        return 1;
    return 0;
}

static inline u32 flags16(struct SceneActorRegistryActor *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline u32 flags32(struct SceneActorRegistryActor *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}

static inline u8 unavailable(struct SceneActorRegistryActor *actor)
{
    if (actor) {
        if (flags16(actor, 0x40) || flags32(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}

static inline u32 test(u32 mask, u32 flags)
{
    return mask & flags;
}

static inline void dispatch(void **callback, void *context)
{
    Callback_InvokeActor(context, *callback);
}

static inline u32 engineFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

static inline u8 pending(struct Skeleton *actor)
{
    u32 mask = 0x10;
    if (actor->actionFlags & mask)
        return 1;
    return 0;
}

s32 Skeleton_UpdateBehaviorCallbacks(struct SceneActorRegistryActor *actor)
{
    struct Skeleton *state = (void *)actor;
    struct SceneActorRegistryControlGroup *block = &state->group.common;
    if (engineFlags(0x400))
        return 0;
    if (test(4, gUnk_02000560))
        return 0;
    if (!(test(2, actor->flags2CC))) {
        if (!sameRegion(&actor->regionId)) {
            SceneActorPlacement_HideActor(actor, 0);
            return 0;
        }
        if (!unavailable(actor) && !pending(state)) {
            if (!(u8)Callback_InvokeActor(actor, actor->activeCallback264))
                Callback_InvokeActor(actor, block->fallback);
        }
        SceneActorBehavior_DispatchPendingFlags(actor);
        SceneActor_UpdateStatus(actor);
        dispatch(&actor->contextCallback274, actor->context);
        SceneActorBehavior_UpdateTimersAndContacts(actor);
        SceneActorMotion_Update(actor);
        SceneActorPlacement_ConsumeShakeFlags(actor);
        SceneActorTarget_OnBindingChanged(actor);
        return 1;
    } else {
        if (!(actor->flags2CC & 1) && sameRegion(&actor->regionId))
            SceneActorPlacement_ShowActor(actor, 0);
        SceneActorTarget_OnBindingChanged(actor);
    }
    return 0;
}

u32 Skeleton_Destroy(struct Skeleton *actor)
{
    Collider_Remove(&actor->bodyCollider);
    SceneActorEffects_ReleaseResources(actor);
    return 0;
}

static inline u8 SkeletonInit_TestSkeletonFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (((struct Skeleton *)actor)->actionFlags & mask)
        return 1;
    return 0;
}

static inline void SkeletonInit_ResetFlags(void *object)
{
    struct Skeleton *actor = object;
    actor->actionFlags = 0;
}

static inline void SkeletonInit_SetFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    ((struct Skeleton *)actor)->actionFlags |= mask;
}

static inline void SkeletonInit_ReadWidgetRelativePalette(struct DisplayKeyframeState *widget)
{
    widget->paletteIndex = Script_GetPc() ? Script_GetValue() + 0x12B : 0x12B;
}

static inline void SkeletonInit_InstallContactDefaults(struct SceneActorRegistryResetView *instance)
{
    instance->contactCallback = Skeleton_ReactToContact;
    MonsterInit_WriteDataPointer(&instance->commandTable.opaque, gUnk_086060FC);
    instance->field238 = SceneActorReaction_SinkThenDefeat;
}

static inline void SkeletonInit_EnableInitialFlags(struct SceneActorRegistryResetView *instance)
{
    MonsterInit_SetTargetingFlags(&instance->tracking, 1);
    MonsterInit_SetBehaviorFlags(instance, 0x1600000);
    MonsterInit_SetStatusFlags(instance, 0x8000008);
    MonsterInit_SetMotionFlags(instance, 0x80);
}

static inline void
SkeletonInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *instance)
{
    instance->messageCallback = SceneActorScript_OnCommand;
    instance->updateCallback = Skeleton_UpdateBehaviorCallbacks;
    instance->destroyCallback = Skeleton_Destroy;
}

static inline void SkeletonInit_SetFlagWord(u32 mask, u32 *field)
{
    *field |= mask;
}

s32 Skeleton_Init(void *object)
{
    struct SceneActorRegistryResetView *instance = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &instance->group;
    struct SceneActorBody *body = &instance->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    SkeletonInit_ResetFlags(instance);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    {
        MonsterInit_SetCategory(actor, Script_GetKeywordValue(0x6B, 5));
        if (MonsterInit_GetCategory(actor) == 7)
            SkeletonInit_SetFlagWord(0x40, &((struct Skeleton *)instance)->actionFlags);
    }
    SceneActor_UpdateProfileForCategory(actor);

    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xAE52, 0);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            ((struct ActorSprite *)heap)->priority = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0xAE52, &tmp);
        actor->animationIndex = 0xFF;
    }

    MonsterInit_InitializePursuitBody(instance, &zero);
    SceneActorReaction_ParseOptions(actor);
    SceneActorScript_ReadPParameters(actor);
    SceneActorScript_ReadCallScriptConfig(actor);

    MonsterInit_ReadPalette(actor, 0x13C, 0x146);
    SceneActorControl_InitBehaviorColliders(actor);

    MonsterInit_InstallSequenceCallbacks(
        actor, Skeleton_Update, SceneActorReaction_SelectTileParameter, sub_080BAE04,
        Skeleton_UpdateInactiveState, Skeleton_UpdateIdleState,
        TentacleBody_UpdateReactionTransition, Skeleton_ConvergeDisplacedPosition,
        Skeleton_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
        SceneActorTarget_UpdateTrackedContact, SceneActorTarget_ResolvePendingObjectSkeleton,
        SceneActorReaction_ResolvePlacementDamage, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, &((struct Skeleton *)instance)->routeProbe,
        &((struct Skeleton *)instance)->destination, Skeleton_UpdateDownedState,
        SceneActorReaction_ReturnToRouteEntry, SceneActorReaction_BeginTargetRecovery, 0, 1, body);
    SceneActorMovement_ResolveRoutePosition(instance);
    MonsterInit_SetRouteReset(actor, Skeleton_ResetRouteReaction);
    group->responseCallback = 0;
    MonsterInit_SetReactionGate(actor, SceneActorReaction_UpdateGate);
    group->field5EC = Skeleton_RestorePursuitRoute;
    SkeletonInit_InstallContactDefaults(actor);
    actor->field2E8 = 0;
    ((struct Skeleton *)instance)->healthThreshold = (s16)MonsterInit_GetHealth(actor) >> 1;
    SkeletonInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->colliderA, ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }

    if (flag != 1) {
        MonsterInit_SetState(actor, 0, 0, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
    }

    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_MaskColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);

    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                  actor->drawable);

    MonsterInit_InitializeReceiver(actor);
    Skeleton_InitVariantMotion(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Skeleton_SetMovementControlPreset(actor);
    Skeleton_SetAttackControlTable(actor);
    Skeleton_SetReactionControlTable(actor);

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    actor->saved.privateFlags = ((struct Skeleton *)instance)->actionFlags;

    if (SkeletonInit_TestSkeletonFlags(instance, 0x800)) {
        MonsterInit_SetControl(actor, Skeleton_BeginKnockedFlight, 0x3D);
        MonsterInit_SetState(actor, 3, 0, 0);
        SkeletonInit_SetFlags(instance, 0x10);
        MonsterInit_SetMotionFlags(actor, 0x1800);
        if (Script_SeekToKeyword(0x70)) {
            actor->body.position.x = MonsterInit_ReadHalfword(0);
            actor->body.position.y = MonsterInit_ReadHalfword(0);
            actor->body.position.z = MonsterInit_ReadHalfword(0);
            actor->body.direction = MonsterInit_ReadByte(0);
            SkeletonInit_ReadWidgetRelativePalette(
                (void *)(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile));
            actor->terrainCell = MonsterInit_CellIndex(MonsterInit_BodyPosition(&actor->body));
            actor->groundHeight2BE = MonsterInit_TerrainHeight(
                MonsterInit_GetTerrainCell(actor), MonsterInit_BodyPosition(&actor->body));
            actor->field2EA = actor->body.position.y - actor->groundHeight2BE;
            actor->body.position.y = actor->groundHeight2BE;
        }
    } else if (SkeletonInit_TestSkeletonFlags(instance, 8)) {
        MonsterInit_SetControl(actor, SceneActorReaction_TickHiddenTimeout, 0x39);
        MonsterInit_SetState(actor, 3, 0, 0);
        MonsterInit_SetMotionFlags(actor, 0x1800);
        SkeletonInit_SetFlags(instance, 0x10);
    }
    if (MonsterInit_GetCategory(actor) == 8)
        MonsterInit_SetMotionFlags(actor, 0x800);

    SkeletonInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor((struct SceneActorRegistryActor *)actor))
        return -1;
    return 0;
}

void Skeleton_Create(void)
{
    void *m = Heap_Alloc(0x74c);
    if (m != 0) {
        Memory_Clear(m, 0x74c);
        if (Skeleton_Init(m) < 0) {
            Skeleton_Destroy(m);
            Heap_Free(m);
        }
    }
}

/* Actor_StepTowardTargetAxis: steps toward a target point along one axis (the larger delta, or the current facing on ties); turns first if the axis changes */

static inline s32 Abs(s32 x)
{
    return x < 0 ? -x : x;
}

u32 Actor_StepTowardTargetAxis(struct SceneActorRegistryResetView *actor,
                               struct ActorSprite *target, u32 nearX, u32 nearZ, s32 speed)
{
    struct SceneActorRegistryResetView *state = actor->context;
    s32 dx = target->x - actor->body.position.x;
    s32 dz = target->z - actor->body.position.z;
    u8 angle;

    if ((u32)Abs(dx) <= nearX && (u32)Abs(dz) <= nearZ) {
        if (Abs(dx) > Abs(dz))
            dz = 0;
        else
            dx = 0;
        state->direction68E = Math_Atan2(dx, dz);
        actor->body.motion.x += (gUnk_086149C4[(state->direction68E + 0x40) & 0xFF] * speed) >> 16;
        actor->body.motion.z += (gUnk_086149C4[state->direction68E] * speed) >> 16;
        return 1;
    }
    if (dx || dz) {
        if (Abs(dx) > Abs(dz))
            dz = 0;
        else if (Abs(dz) > Abs(dx))
            dx = 0;
        else
            switch (((state->direction68E + 0x20) & 0xFF) >> 6) {
                case 0:
                case 2:
                    dz = 0;
                    break;
                default:
                    dx = 0;
                    break;
            }
        angle = Math_Atan2(dx, dz);
        if (angle != state->direction68E) {
            actor->body.direction = angle;
            state->direction68E = angle;
            return 1;
        }
        actor->body.motion.x += (gUnk_086149C4[(angle + 0x40) & 0xFF] * speed) >> 16;
        actor->body.motion.z += (gUnk_086149C4[angle] * speed) >> 16;
        actor->body.direction = angle;
        state->direction68E = angle;
    }
    return 0;
}

s32 SceneActorTerrain_CompareNeighborHeight(s32 cell, s32 height, s32 direction)
{
    s32 slope = -1;
    s32 nextHeight, kind, delta;
    switch (direction) {
        case 0:
            ++cell;
            slope = 2;
            break;
        case 1:
            cell += SceneActorTerrain_Width();
            slope = 1;
            break;
        case 2:
            --cell;
            slope = 2;
            break;
        case 3:
            cell -= SceneActorTerrain_Width();
            slope = 1;
            break;
    }
    nextHeight = *SceneActorTerrain_GetCell(cell) & 15;
    kind = *SceneActorTerrain_GetCell(cell) >> 4;
    {
        u32 flags = ((struct SceneActorChaseTerrainCell *)SceneActorTerrain_GetCell(cell))->flags;
        u32 mask = 3;
        if (flags & mask) {
            if (nextHeight >= height)
                return 1;
            return 2;
        }
    }
    delta = nextHeight - height;
    if (delta == 1 && kind == slope)
        return 0;
    if (delta == -1 && kind == slope)
        return 0;
    if (delta > 0)
        return 1;
    if (delta < 0)
        return 2;
    return 0;
}

void Skeleton_StepPosition(struct ContactPosition *position, s32 n)
{
    u32 val;

    switch (n) {
        case 0:
            val = (((u16)position->x + 0xFF) & 0xFFFFFF00) + 0x80;
            position->x = val;
            break;
        case 1:
            val = (((u16)position->z + 0xFF) & 0xFFFFFF00) + 0x80;
            position->z = val;
            break;
        case 2:
            val = (((u16)position->x - 0xFF) & 0xFFFFFF00) + 0x80;
            position->x = val;
            break;
        case 3:
            val = (((u16)position->z - 0xFF) & 0xFFFFFF00) + 0x80;
            position->z = val;
            break;
    }
}
