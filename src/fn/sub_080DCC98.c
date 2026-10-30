#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"

struct MummyResourceBlock {
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

struct MummyInitContext {
    u8 unknown0[0x6CC];
    u16 field6CC, field6CE;
    u8 unknown6D0[8];
    u32 field6D8, field6DC;
};

struct MummyInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0xC4];
    u16 field6E8;
    u8 unknown6EA[8];
    u16 field6F2;
};

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080D7568(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline void MummyInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 MummyInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}

static inline void MummyInit_ResetFlags(void *object)
{
    struct MummyInstanceState *actor = object;
    actor->field6F2 = 0;
}

static inline void MonsterInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline void MummyInit_InitializeBody(void *object, u32 *fill)
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
    arg0->flags2C0 = Script_GetKeywordValue(0x4D, 0);
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
        group->halfHealth = MummyInit_Half(MonsterInit_GetInitialHealth(arg0));
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
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
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

struct MummyInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MummyInit_SetWidgetScriptPalette(struct MummyInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void MummyInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
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
        MummyInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                         SceneActorScript_GetManagerField62());
}

static inline void MummyInit_InstallPrimary(struct SceneActorRegistryResetView *arg0,
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

static inline void MummyInit_SetRouteReset(struct SceneActorRegistryResetView *actor,
                                           void *callback)
{
    actor->callback248 = callback;
}
static inline void MummyInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                             void *callback)
{
    actor->field23C = callback;
}

void Mummy_InitProfile(void *);
void Mummy_InitCollision(void *);
void Mummy_InitActionHandlers(void *);
void Mummy_SetBehaviorTable(void *);
s32 Mummy_OnUpdate(void *);
u16 Mummy_GetAnimationStepOffset(void *);
void Mummy_TurnTowardRoute(void *);
void Mummy_UpdateWalking(void *);
void Mummy_UpdateIdle(void *);
void Mummy_UpdateDisplacementRecovery(void *);
void Mummy_UpdateDefeat(void *);
void Mummy_UpdateHiddenRecovery(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Mummy_UpdateDying(void);
void Mummy_ResumeSavedTarget(void);
void Mummy_OnUnusedState(void);
void Mummy_ResetRevivalMotion(void);
void SceneActor_EnterMotionPhase(void);
u8 Mummy_SelectMovementResponse(void *);
void Mummy_OnClearActivityFlag(void);
void Mummy_OnMovementUpdate(void);
void Mummy_OnInteractionUpdate(void);
void Mummy_OnReleaseResources(void);

static inline void MummyInit_SetContactCallback(struct SceneActorRegistryResetView *actor,
                                                void *callback)
{
    actor->contactCallback = callback;
}

static inline void MummyInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x600000);
    MonsterInit_SetStatusFlags(actor, 0x8000000);
}

static inline void MummyInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
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

static inline struct MummyInitContext *MummyInit_Context(struct SceneActorRegistryResetView *actor)
{
    return actor->context;
}

static inline void MummyInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Mummy_OnInteractionUpdate;
    arg0->destroyCallback = Mummy_OnReleaseResources;
}

static inline void MummyInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void MummyInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                              struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct MummyResourceBlock *)drawable = *(struct MummyResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void MummyInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                    u32 selected, u32 alternate, u32 current)
{
    actor->alternateOffset = selected;
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

void Mummy_SetControlPreset(void *);
void SceneActorEffects_InitAtActorPosition(void *);
void sub_080D1644(void);
void sub_080D453C(void);

s32 Mummy_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    MummyInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 1);
    Mummy_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MummyInit_InitializeSprite(heap, 0x48DE, 0xA4B2);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MummyInit_InitializeWidget(heap, 0xCB05, 0x48DE, &tmp);
        actor->animationIndex = 0xFF;
    }
    MummyInit_InitializeBody(arg0, &zero);
    MummyInit_ReadPalette(actor, 0x23, 0x192);
    MummyInit_InitializePaletteRange(actor, MonsterInit_GetPaletteBase(actor) + 3, 4, 0);
    Mummy_InitCollision(actor);
    MummyInit_InstallPrimary(actor, Mummy_OnUpdate, Mummy_GetAnimationStepOffset,
                             Mummy_TurnTowardRoute, Mummy_UpdateWalking, Mummy_UpdateIdle,
                             Mummy_UpdateHiddenRecovery, Mummy_UpdateDisplacementRecovery,
                             Mummy_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_080D7568,
                             SceneActorTarget_ResolvePendingObject, sub_081363F4,
                             SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, (u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x718, Mummy_UpdateDying, Mummy_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Mummy_OnUnusedState, 3, &actor->body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MummyInit_SetRouteReset(actor, Mummy_ResetRevivalMotion);
    MummyInit_SetReactionGate(actor, SceneActor_EnterMotionPhase);
    MummyInit_SetContactCallback(actor, Mummy_SelectMovementResponse);
    group->responseCallback = (u32)Mummy_OnClearActivityFlag;
    group->field5EC = Mummy_OnMovementUpdate;
    group->routeChoiceControl = (u32)sub_080D1644;
    group->common.fallback = sub_080D453C;
    actor->field2E8 = 1;
    MummyInit_EnableInitialFlags(actor);
    MummyInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorEffects_InitAtActorPosition(arg0);
    SceneActorTerrain_ReadScriptParameters(actor);
    MummyInit_ReadInitialMode(arg0, group);
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
        MummyInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Mummy_SetControlPreset(actor);
    Mummy_InitActionHandlers(actor);
    Mummy_SetBehaviorTable(actor);

    ((struct MummyInstanceState *)arg0)->field6E8 = actor->tracking.x;
    {
        struct MummyInitContext *context = MummyInit_Context(actor);
        if (Script_SeekToKeyword(0x42)) {
            context->field6CC = MonsterInit_ReadHalfword(0x14);
            context->field6CE = MonsterInit_ReadHalfword(0xB4);
        } else {
            context->field6CC = 0x14;
            context->field6CE = 0xB4;
        }
    }
    {
        struct MummyInitContext *context = MummyInit_Context(actor);
        context->field6D8 = Div(actor->tracking.range * 3, 10);
        context->field6DC = actor->tracking.range;
    }

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    MummyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
