#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void sub_08080BE0(void);
u32 SceneActorReaction_OnEnemyDefault(void);
void sub_081363F4(void);
void SceneActorScript_OnCommand(void);
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

struct VornInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0x68];
    u8 field68C;
    u8 unknown68D[0x2C];
    u8 field6B9;
    u8 unknown6BA[0x1A];
    u16 field6D4;
};

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080B3C1C(void *);
void SceneActorPlacement_ResetStateThree(void);
extern const u8 gUnk_08606038[];

static inline void VornInit_BeginBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    MonsterInit_SetState(actor, 0, 0, behavior);
}
static inline void VornInit_ResetFlags(void *object)
{
    struct VornInstanceState *actor = object;
    actor->field6D4 = 0;
}

static inline void VornInit_InitializeBody(void *object, u32 *fill)
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
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
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

static inline void VornInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                        u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MonsterInit_GetPaletteOffset(actor) + base + 1;
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

static inline void VornInit_SetRouteReset(struct SceneActorRegistryResetView *actor, void *callback)
{
    actor->callback248 = callback;
}
static inline void VornInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                            void *callback)
{
    actor->field23C = callback;
}

void Vorn_InitActorProfile(void *);
void SceneActor_InitContextColliders(void *);
s32 Vorn_OnUpdate(void *);
u16 Vorn_GetAnimationEvent(void *);
void Vorn_UpdateIdleFacing(void *);
void Vorn_ResetStateEntry(void *);
void Vorn_BeginIdle(void *);
void sub_080AD75C(void *);
void Vorn_UpdateDefeat(void *);
void Vorn_UpdateHiddenRecovery(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Vorn_OnReactionNoOp(void);
void SceneActorReaction_ResumeSavedTarget(void);
void SceneActor_ResetContextAnimation(void);
void sub_080B197C(void);
u8 Vorn_SelectHitRecovery(void *);
void Vorn_OnAnimationCleanup(void);
void Vorn_BeginTargetPursuit(void);
void Vorn_OnRender(void);
void Vorn_OnDestroy(void);

static inline void VornInit_SetReactionTable(struct SceneActorRegistryResetView *actor,
                                             const void *table)
{
    MonsterInit_WriteDataPointer(&actor->commandTable.opaque, table);
}
static inline void VornInit_SetContactCallback(struct SceneActorRegistryResetView *actor,
                                               void *callback)
{
    actor->contactCallback = callback;
}

static inline void VornInit_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x1600000);
    MonsterInit_SetStatusFlags(actor, 0x8000000);
}

static inline void VornInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
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

static inline void VornInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Vorn_OnRender;
    arg0->destroyCallback = Vorn_OnDestroy;
}

void Vorn_ParseField624(void *);
void Vorn_InitScriptCallback(void *);
void Vorn_InitActionCooldown(void *);
void Vorn_InitTimedAction(void *);
void Vorn_ParseField69A(void *);
void Vorn_ResetRecoveryMode(void *);
void Vorn_InitMotionLimits(void *);
void Vorn_ParseField688(void *);
void Vorn_InitAttackActionTable(void *);
void Vorn_InitSpecialActionTable(void *);
void Vorn_InitReactionActionTable(void *);
void Vorn_InitActionTable(void *);
void ActorShadow_Disable(void *);
void SceneActorReaction_BeginSideStep(void);
s32 Vorn_TryRestorePatrolRoute(void *);
void Vorn_UpdateKnockbackRecovery(void);
void SceneActorReaction_ResumeOrRunScript(void);
void Vorn_ResumeReactionRoute(void);

s32 Vorn_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 mode;

    VornInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    MonsterInit_SetCategory(actor, 0x19);
    Vorn_InitActorProfile(actor);
    Vorn_ParseField624(actor);
    Vorn_InitScriptCallback(actor);
    Vorn_InitActionCooldown(actor);
    Vorn_InitTimedAction(actor);
    Vorn_ParseField69A(actor);
    Vorn_ResetRecoveryMode(actor);
    Vorn_InitMotionLimits(actor);
    Vorn_ParseField688(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x3DB6, 0);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x3DB6, &tmp);
        actor->animationIndex = 0xFF;
    }
    VornInit_InitializeBody(arg0, &zero);
    VornInit_ReadPalette(actor, 0x31B, 0x192);
    SceneActor_InitContextColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        actor, Vorn_OnUpdate, Vorn_GetAnimationEvent, Vorn_UpdateIdleFacing, Vorn_ResetStateEntry,
        Vorn_BeginIdle, Vorn_UpdateHiddenRecovery, sub_080AD75C, Vorn_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, sub_080B3C1C, SceneActorTarget_ResolvePendingObject,
        sub_081363F4, SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(group, (u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x718,
                                          Vorn_OnReactionNoOp, SceneActorReaction_ResumeSavedTarget,
                                          Vorn_TryRestorePatrolRoute, 0, 1, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    VornInit_SetRouteReset(actor, SceneActor_ResetContextAnimation);
    VornInit_SetReactionGate(actor, sub_080B197C);
    VornInit_SetContactCallback(actor, Vorn_SelectHitRecovery);
    VornInit_SetReactionTable(actor, gUnk_08606038);
    group->contactControl = Vorn_UpdateKnockbackRecovery;
    group->field5E0 = SceneActorReaction_ResumeOrRunScript;
    group->responseCallback = Vorn_OnAnimationCleanup;
    group->field5EC = Vorn_BeginTargetPursuit;
    group->field614 = Vorn_ResumeReactionRoute;
    actor->field2E8 = 3;
    VornInit_EnableInitialFlags(actor);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    VornInit_ReadInitialMode(actor, group);
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
        MonsterInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorRegistry_RegisterCategoryReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Vorn_InitAttackActionTable(actor);
    Vorn_InitSpecialActionTable(actor);
    Vorn_InitReactionActionTable(actor);
    Vorn_InitActionTable(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);

    flag = ((struct VornInstanceState *)arg0)->field68C;
    if (flag == 1) {
        if (MonsterInit_GetDrawableKind(actor) == 0)
            *(u32 *)actor->drawable |= flag;
        else {
            u8 *widget = actor->drawable + 0x20;
            *(u32 *)(widget + 8) |= flag;
        }
        ActorShadow_Disable((u8 *)actor + 0x320);
        MonsterInit_SetState(actor, 3, 0, 0);
        MonsterInit_SetControl(actor, actor->field220, 0xC);
    } else if ((mode = Script_GetKeywordValue(0x54, 0)) != 0) {
        if (mode == 1)
            ((struct VornInstanceState *)arg0)->field6B9 = 0;
        else
            ((struct VornInstanceState *)arg0)->field6B9 = 1;
        VornInit_BeginBehavior(actor, 0x12);
        MonsterInit_SetControl(actor, actor->savedTarget, 0);
        MonsterInit_SetMotionControl(actor, SceneActorReaction_BeginSideStep, 0x26);
    }
    VornInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
