#include "scene_actor_init_dispatch.h"
#include "global.h"
#include "scene_actor_init_group.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"

void Mimic_InitProfile(void *);
void SceneActor_InitializeReactionColliders(void *);
void Mimic_UpdateBehavior(void);
void SceneActor_EnterFlaggedEntryPhase(void);
void Mimic_OnAnimationStateChanged(void);
void Mimic_ConvergeAndTurn(void);
void Mimic_UpdateDefeatBlink(void);
void SceneActor_AcquireCollisionTarget(void);
void Mimic_StartHitReaction(void);
void Mimic_OnBehaviorComplete(void);
void Mimic_ResetPursuitDrawable(void);
void SceneActor_PrepareStationaryReaction(void);
void SceneActor_RestoreSavedTargetOnCompletion(void);
void Mimic_InitBehaviorTable(void *);
void Mimic_InitActionTable(void *);
void Mimic_OnUpdate(void);
void Mimic_ReleaseResources(void);

void Memory_Clear(void *, u32);
void SceneActorReaction_OnEnemyDefault(void);
u8 sub_0807F81C(void *);
void SceneActorTarget_ResolvePendingObject(void);
void sub_081363F4(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
void SceneActorScript_OnCommand(void);
void sub_08080BE0(void);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);

void *Heap_Alloc(u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Anim_Step(void *, void *);
u8 SceneActorRegistry_LinkActor(void *);

struct MimicInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[0x2C];
    struct ContactPosition origin650;
    u16 originCell658;
    u8 unknown65A[4];
    u8 parameter65E;
    u8 unknown65F;
    s32 halfHealth660;
};

extern u8 *gUnknown_02000580;

static inline void MimicInit_InitializeBody(void *object, u32 *fill)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, variant;
    struct SceneActorRegistryRoute *rt;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword('M', 0);
    arg0->field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue('N', 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword('l')) {
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
    if (Script_GetKeywordValue('b', 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword('F', 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword('R', 0);

    if (Script_SeekToKeyword('A')) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword('X')) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword('K', 2);
    variant = (s16)Script_GetKeywordValue('V', -1);
    if (variant < 0)
        variant = 0xFFFF;
    arg0->field9A = variant;
    if (Script_GetKeywordValue('H', 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue('L', 0))
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

static inline void MimicInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                         u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue('C', 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MonsterInit_GetPaletteOffset(actor) + base;
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

static inline void MimicInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Mimic_OnUpdate;
    arg0->destroyCallback = Mimic_ReleaseResources;
}

static inline void MimicInit_SetPaletteBank(struct SceneActorRegistryResetView *actor, u16 palette)
{
    actor->paletteBank = palette;
}
static inline void MimicInit_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = Mimic_ResetPursuitDrawable;
    actor->field23C = SceneActor_PrepareStationaryReaction;
}

static inline void MimicInit_InitializeTerrainCell(struct SceneActorRegistryResetView *actor,
                                                   s32 cell)
{
    actor->terrainCell = cell;
    actor->previousTerrainCell = cell;
}

s32 Mimic_Init(void *object)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 lift;

    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 10);
    Mimic_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x3876, 0x9AF2);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x3876, &tmp);
        actor->animationIndex = 0xFF;
    }
    MimicInit_InitializeBody(arg0, &zero);
    ((struct MimicInstanceState *)arg0)->halfHealth660 = MonsterInit_GetHealth(actor) >> 1;
    actor->body.direction = Script_GetKeywordValue('d', 0);
    ((struct MimicInstanceState *)arg0)->parameter65E = Script_GetKeywordValue('s', 10);
    if (Script_SeekToKeyword('p')) {
        s32 cell;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        MimicInit_InitializeTerrainCell(actor, MonsterInit_CellIndex(&actor->body.position));
        actor->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &actor->body.position);
        actor->field2EA = 0;
        actor->body.position.y = actor->groundHeight2BE;
        MonsterInit_DrawablePosition(actor);
    }
    ((struct MimicInstanceState *)arg0)->origin650 = actor->body.position;
    ((struct MimicInstanceState *)arg0)->originCell658 = actor->terrainCell;
    MimicInit_ReadPalette(actor, 0x304, 0x192);
    MimicInit_SetPaletteBank(actor, 0x306);
    SceneActor_InitializeReactionColliders(actor);
    MonsterInit_InstallOpaqueContactCallbacks(
        actor, Mimic_UpdateBehavior, 0, 0, SceneActor_EnterFlaggedEntryPhase,
        Mimic_OnAnimationStateChanged, 0, Mimic_ConvergeAndTurn, Mimic_UpdateDefeatBlink,
        SceneActorReaction_OnEnemyDefault, SceneActor_AcquireCollisionTarget,
        SceneActorTarget_ResolvePendingObject, sub_081363F4, Mimic_StartHitReaction);
    MonsterInit_InitializeGroupWithBounds(group, (u8 *)arg0 + 0x624, (u8 *)arg0 + 0x648, 0, 0,
                                          Mimic_OnBehaviorComplete, 0, 1, body);
    MimicInit_InstallRoutes(actor);
    MonsterInit_SetBehaviorFlags(actor, 0x400200);
    MonsterInit_SetStatusFlags(actor, 8);
    MonsterInit_SetMotionFlags(actor, 0x40);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
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
        MonsterInit_ClearColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    lift = Script_GetKeywordValue('f', 0);
    if (lift) {
        actor->field2EA = lift;
        MonsterInit_SetControl(actor, SceneActor_RestoreSavedTargetOnCompletion, 0x26);
        MonsterInit_SetState(actor, 3, 0, 0);
    }
    SceneActorBehavior_InstallCallbackTables(actor);
    Mimic_InitBehaviorTable(actor);
    Mimic_InitActionTable(actor);
    actor->field2E8 = 0;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    MimicInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
