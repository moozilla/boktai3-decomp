#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"

struct SkeletonResourceBlock {
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

struct SkeletonInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0xB8];
    u32 field6DC;
    u8 unknown6E0[0x38];
    u32 field718;
};

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void SceneActor_UpdateProfileForCategory(void *);
void SceneActorReaction_ParseOptions(void *);
void SceneActorScript_ReadPParameters(void *);
void SceneActorScript_ReadCallScriptConfig(void *);
void SceneActorControl_InitBehaviorColliders(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void Skeleton_InitVariantMotion(void *);
void Skeleton_SetMovementControlPreset(void *);
void Skeleton_SetAttackControlTable(void *);
void Skeleton_SetReactionControlTable(void *);
s32 Skeleton_Update(void *);
u16 SceneActorReaction_SelectTileParameter(void *);
void sub_080BAE04(void *);
void Skeleton_UpdateInactiveState(void *);
void Skeleton_UpdateIdleState(void *);
void Skeleton_ConvergeDisplacedPosition(void *);
void Skeleton_UpdateDefeat(void *);
u8 sub_0807F81C(void *);
void TentacleBody_UpdateReactionTransition(void *);
void SceneActorTarget_ResolvePendingObjectSkeleton(void);
void SceneActorPlacement_ResetStateThree(void);
void Skeleton_UpdateDownedState(void);
void SceneActorReaction_ReturnToRouteEntry(void);
void SceneActorReaction_BeginTargetRecovery(void);
void Skeleton_ResetRouteReaction(void);
void SceneActorReaction_UpdateGate(void);
void Skeleton_RestorePursuitRoute(void);
u8 Skeleton_ReactToContact(void *);
void SceneActorReaction_SinkThenDefeat(void);
void Skeleton_BeginKnockedFlight(void);
void SceneActorReaction_TickHiddenTimeout(void);
void Skeleton_UpdateBehaviorCallbacks(void);
void Skeleton_Destroy(void);
extern const u8 gUnk_086060FC[];

static inline void SkeletonInit_ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 SkeletonInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}

static inline u8 SkeletonInit_TestSkeletonFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (((struct SkeletonInstanceState *)actor)->field6DC & mask)
        return 1;
    return 0;
}

static inline void SkeletonInit_ResetFlags(void *object)
{
    struct SkeletonInstanceState *actor = object;
    actor->field6DC = 0;
}
static inline void SkeletonInit_SetFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    ((struct SkeletonInstanceState *)actor)->field6DC |= mask;
}

static inline void MonsterInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline void SkeletonInit_InitializeBody(void *object, u32 *fill)
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
        group->halfHealth = SkeletonInit_Half(MonsterInit_GetInitialHealth(arg0));
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

struct SkelWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void SkeletonInit_SetWidgetScriptPalette(struct SkelWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}
static inline void SkeletonInit_ReadWidgetRelativePalette(struct SkelWidget *widget)
{
    widget->paletteBank = Script_GetPc() ? Script_GetValue() + 0x12B : 0x12B;
}

static inline void SkeletonInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
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
        SkeletonInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                            SceneActorScript_GetManagerField62());
}

static inline void SkeletonInit_InstallPrimary(struct SceneActorRegistryResetView *arg0,
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

static inline void SkeletonInit_SetRouteReset(struct SceneActorRegistryResetView *actor,
                                              void *callback)
{
    actor->callback248 = callback;
}
static inline void SkeletonInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                                void *callback)
{
    actor->field23C = callback;
}

static inline void SkeletonInit_InstallContactDefaults(struct SceneActorRegistryResetView *arg0)
{
    arg0->contactCallback = Skeleton_ReactToContact;
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086060FC);
    arg0->field238 = SceneActorReaction_SinkThenDefeat;
}

static inline void SkeletonInit_EnableInitialFlags(struct SceneActorRegistryResetView *arg0)
{
    MonsterInit_SetTargetingFlags(&arg0->tracking, 1);
    MonsterInit_SetBehaviorFlags(arg0, 0x1600000);
    MonsterInit_SetStatusFlags(arg0, 0x8000008);
    MonsterInit_SetMotionFlags(arg0, 0x80);
}

static inline void SkeletonInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
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

static inline void SkeletonInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Skeleton_UpdateBehaviorCallbacks;
    arg0->destroyCallback = Skeleton_Destroy;
}

static inline void SkeletonInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void SkeletonInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                                 struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct SkeletonResourceBlock *)drawable = *(struct SkeletonResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}
static inline void SkeletonInit_SetFlagWord(u32 mask, u32 *field)
{
    *field |= mask;
}

s32 Skeleton_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    SkeletonInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    {
        MonsterInit_SetCategory(actor, Script_GetKeywordValue(0x6B, 5));
        if (MonsterInit_GetCategory(actor) == 7)
            SkeletonInit_SetFlagWord(0x40, &((struct SkeletonInstanceState *)arg0)->field6DC);
    }
    SceneActor_UpdateProfileForCategory(actor);

    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        SkeletonInit_InitializeSprite(heap, 0xAE52, 0);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        SkeletonInit_InitializeWidget(heap, 0xCB05, 0xAE52, &tmp);
        actor->animationIndex = 0xFF;
    }

    SkeletonInit_InitializeBody(arg0, &zero);
    SceneActorReaction_ParseOptions(actor);
    SceneActorScript_ReadPParameters(actor);
    SceneActorScript_ReadCallScriptConfig(actor);

    SkeletonInit_ReadPalette(actor, 0x13C, 0x146);
    SceneActorControl_InitBehaviorColliders(actor);

    SkeletonInit_InstallPrimary(actor, Skeleton_Update, SceneActorReaction_SelectTileParameter,
                                sub_080BAE04, Skeleton_UpdateInactiveState,
                                Skeleton_UpdateIdleState, TentacleBody_UpdateReactionTransition,
                                Skeleton_ConvergeDisplacedPosition, Skeleton_UpdateDefeat,
                                SceneActorReaction_OnEnemyDefault, sub_0807F81C,
                                SceneActorTarget_ResolvePendingObjectSkeleton, sub_081363F4,
                                SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, (u8 *)arg0 + 0x71C, (u8 *)arg0 + 0x740, Skeleton_UpdateDownedState,
        SceneActorReaction_ReturnToRouteEntry, SceneActorReaction_BeginTargetRecovery, 0, 1, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    SkeletonInit_SetRouteReset(actor, Skeleton_ResetRouteReaction);
    group->responseCallback = 0;
    SkeletonInit_SetReactionGate(actor, SceneActorReaction_UpdateGate);
    group->field5EC = Skeleton_RestorePursuitRoute;
    SkeletonInit_InstallContactDefaults(actor);
    actor->field2E8 = 0;
    ((struct SkeletonInstanceState *)arg0)->field718 = (s16)MonsterInit_GetHealth(actor) >> 1;
    SkeletonInit_EnableInitialFlags(actor);
    SkeletonInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    SkeletonInit_ReadInitialMode(actor, group);
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
        SkeletonInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);

    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);

    MonsterInit_InitializeReceiver(actor);
    Skeleton_InitVariantMotion(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Skeleton_SetMovementControlPreset(actor);
    Skeleton_SetAttackControlTable(actor);
    Skeleton_SetReactionControlTable(actor);

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    actor->saved.privateFlags = ((struct SkeletonInstanceState *)arg0)->field6DC;

    if (SkeletonInit_TestSkeletonFlags(arg0, 0x800)) {
        MonsterInit_SetControl(actor, Skeleton_BeginKnockedFlight, 0x3D);
        MonsterInit_SetState(actor, 3, 0, 0);
        SkeletonInit_SetFlags(arg0, 0x10);
        MonsterInit_SetMotionFlags(actor, 0x1800);
        if (Script_SeekToKeyword(0x70)) {
            actor->body.position.x = MonsterInit_ReadHalfword(0);
            actor->body.position.y = MonsterInit_ReadHalfword(0);
            actor->body.position.z = MonsterInit_ReadHalfword(0);
            actor->body.direction = MonsterInit_ReadByte(0);
            SkeletonInit_ReadWidgetRelativePalette((void *)(actor->drawable + 0x20));
            actor->terrainCell = MonsterInit_CellIndex(MonsterInit_BodyPosition(&actor->body));
            actor->groundHeight2BE = MonsterInit_TerrainHeight(
                MonsterInit_GetTerrainCell(actor), MonsterInit_BodyPosition(&actor->body));
            actor->field2EA = actor->body.position.y - actor->groundHeight2BE;
            actor->body.position.y = actor->groundHeight2BE;
        }
    } else if (SkeletonInit_TestSkeletonFlags(arg0, 8)) {
        MonsterInit_SetControl(actor, SceneActorReaction_TickHiddenTimeout, 0x39);
        MonsterInit_SetState(actor, 3, 0, 0);
        MonsterInit_SetMotionFlags(actor, 0x1800);
        SkeletonInit_SetFlags(arg0, 0x10);
    }
    if (MonsterInit_GetCategory(actor) == 8)
        MonsterInit_SetMotionFlags(actor, 0x800);

    SkeletonInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
