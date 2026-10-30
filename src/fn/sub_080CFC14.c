#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"

struct GolemResourceBlock {
    u32 w[8];
};

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void sub_08080BE0(void);
void SceneActorReaction_SinkAndRemove(void);
u32 SceneActorReaction_OnEnemyDefault(void);
void sub_081363F4(void);
void SceneActorScript_OnCommand(void);
extern const u8 gUnk_086108B0[];
extern const u8 gUnk_08606400[];
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

struct GolemInitContext {
    u8 unknown0[0x62C];
    u32 rangeSquared62C;
};

struct GolemInstanceState {
    u8 unknown0[0x624];
    u8 field624;
    u8 unknown625[3];
    u32 field628;
    u8 unknown62C[0x54];
    u16 field680, field682, field684, field686;
    u16 unknown688;
    u16 field68A;
    u8 unknown68C;
    u8 field68D;
    u8 unknown68E[5];
    u8 field693;
};

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_0807F81C(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline void GolemInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 GolemInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}

static inline void GolemInit_ResetFlags(void *object)
{
    struct GolemInstanceState *actor = object;
    actor->field628 = 0;
}

static inline void MonsterInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline void GolemInit_InitializeBody(void *object, u32 *fill)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword('M', 0);
    arg0->field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
        gateDuration = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    MonsterInit_InitializeGate(&group->common.gate, gateDuration);
    MonsterInit_ReadRange(&group->common.control);
    MonsterInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = GolemInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword('K', 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->field9A = identity;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->body.position, rt);
        MonsterInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct GolemInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void GolemInit_SetWidgetScriptPalette(struct GolemInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void GolemInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                         u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MonsterInit_GetPaletteOffset(actor) + base;
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        GolemInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                         SceneActorScript_GetManagerField62());
}

static inline void GolemInit_InstallPrimary(struct SceneActorRegistryResetView *arg0,
                                            s32 (*cb0)(void *), u16 (*cb1)(void *),
                                            void (*cb2)(void *), void (*cb3)(void *),
                                            void (*cb4)(void *), void (*hidden)(void *),
                                            void (*cb5)(void *), void (*cb6)(void *),
                                            u32 (*defaultReaction)(void), u8 (*cb270)(void *),
                                            void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(arg0, 0x2000);
    MonsterInit_WriteResultCallback(&arg0->contextCallback, cb0);
    MonsterInit_WriteSequenceQuery(&arg0->animationQuery.sequence, cb1);
    MonsterInit_WriteActorAction(&arg0->entryTarget224, cb2);
    MonsterInit_WriteActorAction(&arg0->entryTarget228, cb3);
    MonsterInit_WriteActorAction(&arg0->savedTarget, cb4);
    MonsterInit_WriteActorAction(&arg0->reactionTarget230, cb5);
    MonsterInit_WriteActorAction(&arg0->deathControl, cb6);
    MonsterInit_WriteActorAction(&arg0->field220, hidden);
    MonsterInit_WriteDefaultReaction(&arg0->defaultReaction, defaultReaction);
    MonsterInit_WriteContactPredicate(&arg0->contactQuery.predicate, cb270);
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086108B0);
    arg0->callback248 = 0;
    arg0->update26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->damageCallback = damage;
    arg0->contactCallback = contact;
    arg0->field240 = 0;
    MonsterInit_ClearMotion(arg0);
}

static inline void GolemInit_SetRouteReset(struct SceneActorRegistryResetView *actor,
                                           void *callback)
{
    actor->callback248 = callback;
}
static inline void GolemInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                             void *callback)
{
    actor->field23C = callback;
}

void Golem_InitAnimationProfile(void *);
void Golem_InitColliders(void *);
void Golem_InitActionHandlers(void *);
void Golem_InitSpecialActionHandlers(void *);
s32 Golem_OnUpdate(void *);
u16 Skeleton_LookupAnimationEntry(void *);
void sub_080C5930(void *);
void Golem_UpdateIdleState(void *);
void Golem_UpdateIdle(void *);
void Golem_ResumeBehaviorAfterDisplacement(void *);
void Golem_UpdateDefeat(void *);
void Golem_UpdateRevivalReturn(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Golem_UpdateDying(void);
void Golem_ResumeSavedTarget(void);
void Golem_OnUnusedState(void);
void Golem_ResetCombatState(void);
void SceneActorControl_RefreshMotionRoute(void);
u8 Golem_UpdateHitReaction(void *);
void Golem_ClearControlFlag(void);
void Golem_RestoreRoute(void);
void Golem_OnSharedActorUpdate(void);
void Golem_OnDestroy(void);

static inline void GolemInit_SetContactCallback(struct SceneActorRegistryResetView *actor,
                                                void *callback)
{
    actor->contactCallback = callback;
}

static inline void GolemInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x1600000);
    MonsterInit_SetStatusFlags(actor, 0x8000008);
}

static inline void GolemInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
                                             struct SceneActorChaseGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.effect = 0x21;
    if (i != 0) {
        MonsterInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            MonsterInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline struct GolemInitContext *GolemInit_Context(struct SceneActorRegistryResetView *actor)
{
    return actor->context;
}
static inline void GolemInit_InitializeReceiver(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_ClearReceiver(actor);
    GolemInit_Context(actor)->rangeSquared62C = actor->tracking.range;
}

static inline void GolemInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Golem_OnSharedActorUpdate;
    arg0->destroyCallback = Golem_OnDestroy;
}

static inline void GolemInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void GolemInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                              struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct GolemResourceBlock *)drawable = *(struct GolemResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void GolemInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 alternate, u32 current)
{
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

s32 Golem_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    GolemInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 3);
    Golem_InitAnimationProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        GolemInit_InitializeSprite(heap, 0x5290, 0x1E38);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        GolemInit_InitializeWidget(heap, 0xCB05, 0x5290, &tmp);
        actor->animationIndex = 0xFF;
    }
    GolemInit_InitializeBody(arg0, &zero);
    GolemInit_ReadPalette(actor, 0x27D, 0x192);
    GolemInit_InitializePaletteRange(actor, 2, 0);
    Golem_InitColliders(actor);
    GolemInit_InstallPrimary(
        actor, Golem_OnUpdate, Skeleton_LookupAnimationEntry, sub_080C5930, Golem_UpdateIdleState,
        Golem_UpdateIdle, Golem_UpdateRevivalReturn, Golem_ResumeBehaviorAfterDisplacement,
        Golem_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_0807F81C,
        SceneActorTarget_ResolvePendingObject, sub_081363F4, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, (u8 *)arg0 + 0x638, (u8 *)arg0 + 0x65C, Golem_UpdateDying, Golem_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Golem_OnUnusedState, 1, &actor->body);
    SceneActorMovement_ResolveRoutePosition(actor);
    GolemInit_SetRouteReset(actor, Golem_ResetCombatState);
    GolemInit_SetReactionGate(actor, SceneActorControl_RefreshMotionRoute);
    GolemInit_SetContactCallback(actor, Golem_UpdateHitReaction);
    group->responseCallback = (u32)Golem_ClearControlFlag;
    group->field5EC = Golem_RestoreRoute;
    actor->field2E8 = 0;
    GolemInit_EnableInitialFlags(actor);
    GolemInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    GolemInit_ReadInitialMode(actor, group);
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
        GolemInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    GolemInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Golem_InitActionHandlers(actor);
    Golem_InitSpecialActionHandlers(actor);

    {
        struct GolemInstanceState *instance = (void *)arg0;
        instance->field68D = 0;
        instance->field680 = 0x78;
        instance->field686 = 0;
        instance->field684 = 2;
        instance->field682 = 0;
        instance->field680 = 0xA0;
        instance->field68A = 0;
        instance->field624 = 0;
        instance->field693 = 0;
    }

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    GolemInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
