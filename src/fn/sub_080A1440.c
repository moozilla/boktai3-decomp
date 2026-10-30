#include "scene_body_api.h"
#include "global.h"
#include "scene_actor_state.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_dispatch.h"
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void Crow_InitProfile(void *);
void Crow_Update(void);
u32 Crow_GetTimedAction(void *);
void Crow_PlayEntry2(void);
void Crow_ReadSpawnScript(void *);
void Crow_OnIdleAnimation(void);
void RouteActor_UpdatePeriodicAnimation(void);
void Crow_RecoverFromDisplacement(void);
void SceneActorControl_CompleteScriptedResponse_0809F368(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void Crow_ResetGroundState(void);
void Crow_BeginReaction(void);
void Crow_TryActivatePlayerProximityResponse(void);
void SceneActorScript_OnCommand(void);
void Crow_InitPlayerTarget(void);
void Crow_Destroy(void);
void AnimData_SelectPaletteBank(void *, s32);
void Crow_InitColliders(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void Crow_InitActionTable(void *);
void Crow_SetBehaviorTable(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_08605EC4[];
extern SceneActorCommandHandler const gUnk_086108B0[];
struct CrowActor {
    u8 unknown0[0x3D4];
    u32 flags;
};

static inline void MonsterInit_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit;
    s32 scriptVariant;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
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
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)MonsterInit_ReadKeywordHalfword(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->field9A = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);
}
static inline void Crow_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                    u16 alternate)
{
    s32 r1v;
    u32 currentPalette;
    actor->paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    currentPalette = MonsterInit_GetPaletteOffset(actor);
    currentPalette += 0x1CE;
    MonsterInit_SetAlternatePaletteOffset(actor, (u16)currentPalette);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

static inline void Crow_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = Crow_InitPlayerTarget;
    actor->destroyCallback = Crow_Destroy;
}

static inline void Crow_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_08605EC4);
    actor->callback248 = Crow_ResetGroundState;
    actor->field23C = Crow_BeginReaction;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, Crow_TryActivatePlayerProximityResponse);
}

s32 Crow_Init(void *object)
{
    struct CrowActor *crow = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    crow->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x13);
    Crow_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x5A6E, 0x7B03);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5A6E, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    Crow_ReadSpawnScript(crow);
    Crow_ReadPalette(actor, 0x1CB, 0x192);

    Crow_InitColliders(actor);
    MonsterInit_InstallOpaqueContactCallbacks(
        actor, Crow_Update, Crow_GetTimedAction, Crow_PlayEntry2, Crow_OnIdleAnimation,
        RouteActor_UpdatePeriodicAnimation, 0, Crow_RecoverFromDisplacement,
        SceneActorControl_CompleteScriptedResponse_0809F368, SceneActorReaction_OnEnemyDefault, 0,
        0, SceneActorReaction_ResolveDamage, SceneActorPlacement_PlayResponseAndResetState);
    Crow_InstallRoutes(actor);
    MonsterInit_SetMotionFlags(actor, 0x60);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x02100408);
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
        MonsterInit_MaskColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Crow_InitActionTable(actor);
    Crow_SetBehaviorTable(actor);
    actor->field2E8 = 2;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Crow_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
