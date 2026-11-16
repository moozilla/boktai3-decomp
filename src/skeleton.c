#include "skeleton.h"
#include "golem.h"

/* Resolve contact flags, animation, and pursuit routing. The normal chase
 * continuation returns zero even after installing its control. */

u8 Skeleton_ReactToContact(struct Skeleton *actor)
{
    struct Skeleton *body = actor->chase.base.context;
    struct SceneActorChaseGroup *group = &actor->chase.group;

    if (!(actor->chase.base.field2D8 & 0x20000)) {
        if (actor->chase.base.reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x13E);
    }
    SceneActorScript_TriggerCallOnCount((struct SceneActorRegistryActor *)actor);
    if (SceneActorChase_QueryMotionFlags(&actor->chase, 0x2000)) {
        actor->chase.base.body.direction = actor->chase.base.reactionDirection + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((struct RoomActor *)actor, 0);
        SceneActorChase_ClearMotionFlags(&actor->chase, 0x2020);
        SceneActorChase_SetActorFlags(actor, 0x10);
        group->destination->pad = 0;
        SceneActorChase_TrackPlayer(&actor->chase);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(&actor->chase, group->route,
                                          &actor->chase.base.body.position,
                                          &actor->chase.base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, actor->chase.base.entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->chase.base.field2C2 = Div(0x40, 3);
        return 1;
    }
    if (SceneActorChase_QueryMotionFlags(&actor->chase, 0x8002)) {
        SceneActorChase_ClearMotionFlags(&actor->chase, 0x20);
        return 0;
    }
    if (actor->chase.base.field2DC & 0xC0000) {
        SceneActorChase_SetControl(actor, actor->chase.base.reactionTarget230, 0x12);
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_ClearWordFlags(&actor->chase.base.flags2D0, 0x1000000);
        return 1;
    }
    SceneActorChase_ClearMotionFlags(&actor->chase, 0x20);
    if (SceneActorChase_QueryMotionFlags(&actor->chase, 0x100))
        return 0;
    if (actor->chase.base.field2B8)
        return 0;
    if (SceneActorChase_TrackingFlags(&actor->chase.base.tracking, 0x10))
        return 0;
    if (SceneActorChase_QueryMotionFlags(&actor->chase, 0x80)) {
        if (SceneActorChase_BeginContactAnimation(&actor->chase))
            Sound_PlaySong(0x3A7);
        if (!actor->chase.base.tracking.target)
            return 0;
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_GroupSetFlags(group, 4);
        SceneActorChase_TrackPlayer(&actor->chase);
        if (!SceneActorChase_Arrive(actor)) {
            body->pursuitTimer = 0;
            if (!SceneActorChase_TryRoute(&actor->chase, group->route,
                                          &actor->chase.base.body.position,
                                          &actor->chase.base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return 0;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        actor->chase.base.field2C2 = 0x20;
        return 1;
    }
    if (SceneActorChase_BeginContactAnimation(&actor->chase))
        Sound_PlaySong(0x3A7);
    if (!actor->chase.base.tracking.target)
        return 0;
    SceneActorChase_TrackPlayer(&actor->chase);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!SceneActorChase_Arrive(actor)) {
        body->pursuitTimer = 0;
        if (!SceneActorChase_TryRoute(&actor->chase, group->route, &actor->chase.base.body.position,
                                      &actor->chase.base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
    }
    SceneActorChase_SetControl(actor, actor->chase.base.entryTarget228, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->chase.base.field2C2 = 0x20;
    return 0;
}

/* Skeleton_UpdateProbeApproach: control phase that moves the actor toward its nav probe descriptor (+0x5D0): turns on a probe event, otherwise accelerates; changes state on arrival */

void Skeleton_UpdateProbeApproach(struct Skeleton *actor)
{
    struct SceneActorChaseGroup *nav = &actor->chase.group;
    struct Skeleton *control = actor->chase.base.context;
    s32 speed;
    u32 event;
    u16 *scale;

    if (SceneActorControl_TakeFlag(&actor->chase.base.field2B1)) {
        SceneActorChase_SetControl(actor, Skeleton_UpdateTargetTransition, 0x3B);
        {
            u32 bit = 0x200;
            u32 *flags = &control->actionFlags;
            *flags |= bit;
        }
    }
    if (SceneActorReaction_IsRangeReady(&actor->chase)) {
        if ((u8)Callback_InvokeActor(actor, actor->chase.base.contactQuery.opaque)) {
            return;
        }
        if (sub_0812F3CC(&actor->chase)) {
            return;
        }
    }
    speed = Callback_InvokeActor(*(&actor), actor->chase.base.animationQuery.opaque);
    actor->chase.base.field2C2 = 0x20;
    {
        u32 mask = 0xA02F;
        if (actor->chase.base.flags2D4 & mask) {
            u16 *p = &actor->chase.base.field2C2;
            *p = 0x40;
            scale = p;
        } else {
            u16 *q = &actor->chase.base.field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0) {
                *scale = 0x40;
            }
        }
    }
    if (actor->chase.base.drawableKind == 0)
        Anim_SetDurationScale(
            ((struct SceneActorAnimationSequenceDrawable *)actor->chase.base.drawable)->sequence,
            *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->chase.base.drawable)->tile, *scale);
    event = NavMesh_ConsumeProbeEvent(nav->route);
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, nav->route, 0x20, 0x20, speed)) {
            Skeleton_ReturnToBehavior(&actor->chase, 16);
            SceneActorChase_SetControl(actor, actor->chase.base.savedTarget, 0);
            return;
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, nav->route, 0x20, 0x20,
                                                             speed)) {
        Skeleton_SetState(&actor->chase, 3, 1, 0);
        {
            u32 bit = 0x100;
            u32 *flags = &control->actionFlags;
            *flags |= bit;
        }
        return;
    }
    actor->chase.base.counter2AC++;
}

void Skeleton_UpdateEntryPhase(struct Skeleton *actor)
{
    if (SceneActorControl_TakeFlag(&actor->chase.base.field2B1)) {
        void *control = actor->chase.base.entryTarget224;
        SceneActorChase_SetControl(actor, control, 2);
    }
    if (SceneActorReaction_IsRangeReady(&actor->chase)) {
        if ((u8)Callback_InvokeActor(actor, actor->chase.base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(&actor->chase))
            return;
    }
    if (actor->chase.base.field2B0) {
        Skeleton_ReturnToBehavior(&actor->chase, 15);
        SceneActorChase_SetControl(actor, actor->chase.base.savedTarget, 0);
    } else
        ++actor->chase.base.counter2AC;
}

void Skeleton_UpdateEntryRecoveryPhase(struct Skeleton *actor)
{
    if (SceneActorControl_TakeFlag(&actor->chase.base.field2B1)) {
        void *control = actor->chase.base.entryTarget224;
        SceneActorChase_SetControl(actor, control, 2);
    }
    if (SceneActorReaction_IsRangeReady(&actor->chase)) {
        if ((u8)Callback_InvokeActor(actor, actor->chase.base.contactQuery.opaque))
            return;
        if (sub_0812F3CC(&actor->chase))
            return;
    }
    if (actor->chase.base.field2B0) {
        {
            u32 state = 3;
            u32 phase = 1;
            actor->chase.base.state2A8 = state;
            actor->chase.base.phase2A9 = phase;
            actor->chase.base.behavior2AA = 0;
            actor->chase.base.counter2AC = 0;
            actor->chase.base.field2B1 = 1;
        }
        SceneActorChase_SetControl(actor, (void *)SceneActorApproach_UpdateRange, 60);
    } else
        ++actor->chase.base.counter2AC;
}

void Skeleton_SetMovementControlPreset(struct ChaseActor *actor)
{
    actor->base.callbacks284 = (void **)gUnk_086060B4;
}

s32 Skeleton_Update(struct Skeleton *actor)
{
    u32 m;
    SceneActorReaction_TickHiddenResponse(actor);
    m = 0x2000;
    if ((actor->chase.base.flags2D4 & m) || actor->chase.base.field2B8 == 0) {
        Callback_InvokeActor(actor, actor->chase.base.callbacks27C[actor->chase.base.state2A8]);
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
    struct SceneActorRegistryControlGroup *block = &state->chase.group.common;
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

static inline u8 SkeletonInit_TestSkeletonFlags(struct Skeleton *actor, u32 mask)
{
    if (actor->actionFlags & mask)
        return 1;
    return 0;
}

static inline void SkeletonInit_ResetFlags(void *object)
{
    struct Skeleton *actor = object;
    actor->actionFlags = 0;
}

static inline void SkeletonInit_SetFlags(struct Skeleton *actor, u32 mask)
{
    actor->actionFlags |= mask;
}

static inline void SkeletonInit_ReadWidgetRelativePalette(struct DisplayKeyframeState *widget)
{
    widget->paletteIndex = Script_GetPc() ? Script_GetValue() + 0x12B : 0x12B;
}

static inline void SkeletonInit_InstallContactDefaults(struct ChaseActor *instance)
{
    instance->base.contactCallback = Skeleton_ReactToContact;
    MonsterInit_WriteDataPointer(&instance->base.commandTable.opaque, gUnk_086060FC);
    instance->base.field238 = SceneActorReaction_SinkThenDefeat;
}

static inline void SkeletonInit_EnableInitialFlags(struct ChaseActor *instance)
{
    MonsterInit_SetTargetingFlags(&instance->base.tracking, 1);
    MonsterInit_SetBehaviorFlags(instance, 0x1600000);
    MonsterInit_SetStatusFlags(instance, 0x8000008);
    MonsterInit_SetMotionFlags(instance, 0x80);
}

static inline void SkeletonInit_InstallCompletionCallbacks(struct ChaseActor *instance)
{
    instance->base.messageCallback = SceneActorScript_OnCommand;
    instance->base.updateCallback = Skeleton_UpdateBehaviorCallbacks;
    instance->base.destroyCallback = Skeleton_Destroy;
}

static inline void SkeletonInit_SetFlagWord(u32 mask, u32 *field)
{
    *field |= mask;
}

s32 Skeleton_Init(void *object)
{
    struct Skeleton *instance = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &instance->chase.group;
    struct SceneActorBody *body = &instance->chase.base.body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    SkeletonInit_ResetFlags(instance);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    {
        MonsterInit_SetCategory(actor, Script_GetKeywordValue(0x6B, 5));
        if (MonsterInit_GetCategory(actor) == 7)
            SkeletonInit_SetFlagWord(0x40, &instance->actionFlags);
    }
    SceneActor_UpdateProfileForCategory(actor);

    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xAE52, 0);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            ((struct ActorSprite *)heap)->priority = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0xAE52, &tmp);
        actor->base.animationIndex = 0xFF;
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
        group, &instance->routeProbe, &instance->destination, Skeleton_UpdateDownedState,
        SceneActorReaction_ReturnToRouteEntry, SceneActorReaction_BeginTargetRecovery, 0, 1, body);
    SceneActorMovement_ResolveRoutePosition(instance);
    MonsterInit_SetRouteReset(actor, Skeleton_ResetRouteReaction);
    group->responseCallback = 0;
    MonsterInit_SetReactionGate(actor, SceneActorReaction_UpdateGate);
    group->field5EC = Skeleton_RestorePursuitRoute;
    SkeletonInit_InstallContactDefaults(actor);
    actor->base.field2E8 = 0;
    instance->healthThreshold = (s16)MonsterInit_GetHealth(actor) >> 1;
    SkeletonInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~1);
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
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);

    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
                  actor->base.drawable);

    MonsterInit_InitializeReceiver(actor);
    Skeleton_InitVariantMotion(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Skeleton_SetMovementControlPreset(actor);
    Skeleton_SetAttackControlTable(actor);
    Skeleton_SetReactionControlTable(actor);

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    actor->base.saved.privateFlags = instance->actionFlags;

    if (SkeletonInit_TestSkeletonFlags(instance, 0x800)) {
        MonsterInit_SetControl(actor, Skeleton_BeginKnockedFlight, 0x3D);
        MonsterInit_SetState(actor, 3, 0, 0);
        SkeletonInit_SetFlags(instance, 0x10);
        MonsterInit_SetMotionFlags(actor, 0x1800);
        if (Script_SeekToKeyword(0x70)) {
            actor->base.body.position.x = MonsterInit_ReadHalfword(0);
            actor->base.body.position.y = MonsterInit_ReadHalfword(0);
            actor->base.body.position.z = MonsterInit_ReadHalfword(0);
            actor->base.body.direction = MonsterInit_ReadByte(0);
            SkeletonInit_ReadWidgetRelativePalette(
                (void *)(&((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile));
            actor->base.terrainCell =
                MonsterInit_CellIndex(MonsterInit_BodyPosition(&actor->base.body));
            actor->base.groundHeight = MonsterInit_TerrainHeight(
                MonsterInit_GetTerrainCell(actor), MonsterInit_BodyPosition(&actor->base.body));
            actor->base.field2EA = actor->base.body.position.y - actor->base.groundHeight;
            actor->base.body.position.y = actor->base.groundHeight;
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
    void *m = Heap_Alloc(sizeof(struct Skeleton));
    if (m != 0) {
        Memory_Clear(m, sizeof(struct Skeleton));
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

u32 Actor_StepTowardTargetAxis(struct ChaseActor *actor, struct ActorSprite *target, u32 nearX,
                               u32 nearZ, s32 speed)
{
    struct Golem *state = actor->base.context;
    s32 dx = target->x - actor->base.body.position.x;
    s32 dz = target->z - actor->base.body.position.z;
    u8 angle;

    if ((u32)Abs(dx) <= nearX && (u32)Abs(dz) <= nearZ) {
        if (Abs(dx) > Abs(dz))
            dz = 0;
        else
            dx = 0;
        state->direction68E = Math_Atan2(dx, dz);
        actor->base.body.motion.x +=
            (gUnk_086149C4[(state->direction68E + 0x40) & 0xFF] * speed) >> 16;
        actor->base.body.motion.z += (gUnk_086149C4[state->direction68E] * speed) >> 16;
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
            actor->base.body.direction = angle;
            state->direction68E = angle;
            return 1;
        }
        actor->base.body.motion.x += (gUnk_086149C4[(angle + 0x40) & 0xFF] * speed) >> 16;
        actor->base.body.motion.z += (gUnk_086149C4[angle] * speed) >> 16;
        actor->base.body.direction = angle;
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
