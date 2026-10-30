#include "global.h"
#include "scene_actor_state.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void TentacleActor_InitProfile(void *);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorScript_OnCommand(void);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_086108B0[];
struct TentacleInstanceState {
    u8 unknown0[0x8D0];
    u16 flags;
    u8 unknown8D2[0x14];
    u16 interval8E6;
    u8 unknown8E8[0x44];
    struct ContactPosition savedPosition;
};
void TentacleActor_OnUpdate(void *);
u32 TentacleActor_OnResolveVariantValue(void *);
void TentacleBody_OnIdleState(void *);
void TentacleBody_UpdateAttackWindup(void *);
void TentacleBody_ResetDirectionalPose(void *);
void sub_080A71A0(void *);
void TentacleBody_UpdateDefeat(void *);
void TentacleBody_UpdateHiddenTransition(void *);
void sub_080ABB18(void *);
void ActorTrail_ResetAtSavedPosition(void *);
void SceneActor_ResetQueuedState(void *);
s32 SceneActor_TryRequestFacingReaction(void *);
void sub_080AA744(void *);
void TentacleEnemy_InitPlayerReference(void *);
void TentacleBody_Destroy(void *);
void TentacleActor_ResetRouteHistory(void *);
void TentacleActor_InitCollision(void *);
void TentacleActor_InitSegmentUpdateState(void *);
void TentacleActor_SelectWanderRoute(void *);
void TentacleBody_InitBehaviorTable(void *);
void TentacleActor_InitStateHandlers(void *);
void TentacleBody_InitSegmentAnimations(void *, u32, u32, u32);
void TentacleActor_ResetSegmentPositions(void *, void *);
void TentacleActor_InitSegmentColliders(void *);
void TentacleActor_ActivateMarkedSegment(void *);

static inline void MonsterInit_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit;
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
    scriptVariant = (s16)Script_GetKeywordValue(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->field9A = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->drawableKind ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);
}
static inline void Tentacle_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                        u16 alternate)
{
    s32 palette;
    actor->paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    MonsterInit_SetCurrentPalette(actor, MonsterInit_GetPaletteOffset(actor) + base);
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette((void *)(actor->drawable + 0x20),
                                     SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct SceneActorRegistryResetView *actor, void *cb0,
                                              u32 (*cb1)(void *), void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *targetQuery,
                                              void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->contextCallback = cb0;
    MonsterInit_WriteAnimationQuery(&actor->animationQuery.word, cb1);
    actor->entryTarget224 = cb2;
    actor->entryTarget228 = cb3;
    actor->savedTarget = cb4;
    actor->reactionTarget230 = cb5;
    actor->deathControl = cb6;
    actor->field220 = hidden;
    actor->defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_086108B0);
    actor->callback248 = 0;
    actor->update26C = targetQuery;
    actor->field238 = SceneActorReaction_SinkAndRemove;
    actor->damageCallback = damage;
    actor->contactCallback = contact;
    actor->field240 = 0;
    MonsterInit_ClearMotion(actor);
}
static inline void Tentacle_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = TentacleEnemy_InitPlayerReference;
    actor->destroyCallback = TentacleBody_Destroy;
}

static inline void Tentacle_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = ActorTrail_ResetAtSavedPosition;
    actor->field23C = SceneActor_ResetQueuedState;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, SceneActor_TryRequestFacingReaction);
    actor->field240 = sub_080AA744;
}

static inline void Tentacle_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 8);
    MonsterInit_SetMotionFlags(actor, 0x40);
}
s32 TentacleActor_Init(void *object)
{
    struct TentacleInstanceState *tentacle = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    tentacle->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 9);
    TentacleActor_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD636, 0xC102);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        res = ScriptProc_Start(0xCB05, 0xD636);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);

    if (Script_SeekToKeyword(0x70)) {
        s32 height;
        u16 *ground;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        actor->terrainCell = MonsterInit_CellId(&actor->body.position);
        height =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &actor->body.position);
        ground = &actor->groundHeight2BE;
        *ground = height;
        actor->field2EA = (u16)actor->body.position.y - height;
        actor->body.position.y = *ground;
        if (MonsterInit_GetDrawableKind(actor) == 0)
            *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
        else
            *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
        tentacle->savedPosition = actor->body.position;
    }

    if (Script_SeekToKeyword(0x50))
        tentacle->interval8E6 = Script_GetPc() ? Script_GetValue() : 300;
    TentacleActor_ResetRouteHistory(actor);
    Tentacle_ReadPalette(actor, 0x22A, 0x192);
    TentacleActor_InitCollision(actor);
    TentacleActor_InitSegmentUpdateState(actor);
    MonsterInit_InstallPrimary(actor, TentacleActor_OnUpdate, TentacleActor_OnResolveVariantValue,
                               TentacleBody_OnIdleState, TentacleBody_UpdateAttackWindup,
                               TentacleBody_ResetDirectionalPose,
                               TentacleBody_UpdateHiddenTransition, sub_080A71A0,
                               TentacleBody_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
                               SceneActorReaction_ResolveDamage, sub_080ABB18);
    Tentacle_InstallRoutes(actor);
    Tentacle_EnableInitialFlags(actor);
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
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    TentacleActor_SelectWanderRoute(actor);
    MonsterInit_SetState(actor, 0, 0, 2);
    MonsterInit_SetControl(actor, actor->entryTarget228, 1);
    SceneActorBehavior_InstallCallbackTables(actor);
    TentacleBody_InitBehaviorTable(actor);
    TentacleActor_InitStateHandlers(actor);
    TentacleBody_InitSegmentAnimations(actor, 0xD636, 0xC102, MonsterInit_GetDirection(actor));
    TentacleActor_ResetSegmentPositions(tentacle, &actor->body.position);
    TentacleActor_InitSegmentColliders(actor);
    TentacleActor_SelectWanderRoute(actor);
    TentacleActor_ActivateMarkedSegment(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Tentacle_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
