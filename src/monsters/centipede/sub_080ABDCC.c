#include "scene/collision/scene_body_api.h"
#include "global.h"
#include "scene/actor/scene_actor_state.h"
#include "engine/video/render.h"
#include "engine/engine_globals.h"
#include "engine/video/display_sequence_inline.h"
#include "monsters/scene_actor_init_group.h"
#include "monsters/scene_actor_init_graphics.h"
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void Centipede_InitProfile(void *);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorScript_OnCommand(void *actor, const u16 *command);
void AnimData_SelectPaletteBank(void *, s32);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_086108B0[];
struct TentacleActor {
    struct ChaseActor base;
    u8 unknown624[0x2AC];
    u16 flags;
    u8 unknown8D2[0x14];
    u16 interval8E6;
    u8 unknown8E8[0x44];
    struct ContactPosition savedPosition;
};
void Centipede_OnUpdate(void *);
u32 Centipede_OnResolveVariantValue(void *);
void CentipedeBody_OnIdleState(void *);
void CentipedeBody_UpdateAttackWindup(void *);
void CentipedeBody_ResetDirectionalPose(void *);
void CentipedeBody_ConvergeReactionMotion(void *);
void CentipedeBody_UpdateDefeat(void *);
void CentipedeBody_UpdateHiddenTransition(void *);
void sub_080ABB18(void *);
void ActorTrail_ResetAtSavedPosition(void *);
void SceneActor_ResetQueuedState(void *);
s32 SceneActor_TryRequestFacingReaction(void *);
void sub_080AA744(void *);
void Centipede_InitPlayerReference(void *);
void CentipedeBody_Destroy(void *);
void Centipede_ResetRouteHistory(void *);
void Centipede_InitCollision(void *);
void Centipede_InitSegmentUpdateState(void *);
void Centipede_SelectWanderRoute(void *);
void CentipedeBody_InitBehaviorTable(void *);
void Centipede_InitStateHandlers(void *);
void CentipedeBody_InitSegmentAnimations(void *, u32, u32, u32);
void Centipede_ResetSegmentPositions(void *, void *);
void Centipede_InitSegmentColliders(void *);
void Centipede_ActivateMarkedSegment(void *);

static inline void MonsterInit_InitializeBody(struct ChaseActor *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit;
    s32 scriptVariant;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->base.tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->base.hudKind = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)Script_GetKeywordValue(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->base.hudLabel = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->base.drawableKind ? 0 : arg0->base.drawable);
    SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x40, 0x40);
}
static inline void Tentacle_ReadPalette(struct ChaseActor *actor, u16 base, u16 alternate)
{
    s32 palette;
    actor->base.paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + base);
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->base.drawable + 0x2C, palette);
    else
        *(u8 **)(actor->base.drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette((void *)(actor->base.drawable + 0x20),
                                     SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct ChaseActor *actor, void *cb0,
                                              u32 (*cb1)(void *), void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *targetQuery,
                                              void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->base.onUpdate = cb0;
    MonsterInit_WriteAnimationQuery(&actor->base.animationQuery.word, cb1);
    actor->base.onEntry = cb2;
    actor->base.onAlternateEntry = cb3;
    actor->base.onStanding = cb4;
    actor->base.onReaction = cb5;
    actor->base.onDefeat = cb6;
    actor->base.onHidden = hidden;
    actor->base.defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_086108B0);
    actor->base.routeReset = 0;
    actor->base.targetQuery = targetQuery;
    actor->base.sinkCallback = SceneActorReaction_SinkAndRemove;
    actor->base.onDamage = damage;
    actor->base.onContact = contact;
    actor->base.field240 = 0;
    MonsterInit_ClearMotion(actor);
}
static inline void Tentacle_InstallCompletionCallbacks(struct ChaseActor *actor)
{
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)Centipede_InitPlayerReference;
    actor->base.destroy = (SceneActorResultCallback)CentipedeBody_Destroy;
}

static inline void Tentacle_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.routeReset = ActorTrail_ResetAtSavedPosition;
    actor->base.reactionGate = SceneActor_ResetQueuedState;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque,
                              SceneActor_TryRequestFacingReaction);
    actor->base.field240 = sub_080AA744;
}

static inline void Tentacle_EnableInitialFlags(struct ChaseActor *actor)
{
    MonsterInit_SetTargetingFlags(&actor->base.tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 8);
    MonsterInit_SetMotionFlags(actor, 0x40);
}
s32 Centipede_Init(void *object)
{
    struct TentacleActor *tentacle = object;
    struct ChaseActor *actor = &tentacle->base;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    tentacle->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_TENTACLE);
    Centipede_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD636, 0xC102);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        res = ScriptProc_Start(0xCB05, 0xD636);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);

    if (Script_SeekToKeyword(0x70)) {
        s32 height;
        u16 *ground;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        actor->base.terrainCell = MonsterInit_CellId(&actor->base.body.position);
        height = MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor),
                                           &actor->base.body.position);
        ground = &actor->base.groundHeight;
        *ground = height;
        actor->base.heightOffset = (u16)actor->base.body.position.y - height;
        actor->base.body.position.y = *ground;
        if (MonsterInit_GetDrawableKind(actor) == 0)
            *(struct ContactPosition *)(actor->base.drawable + 0x1C) = actor->base.body.position;
        else
            *(struct ContactPosition *)(actor->base.drawable + 0x40) = actor->base.body.position;
        tentacle->savedPosition = actor->base.body.position;
    }

    if (Script_SeekToKeyword(0x50))
        tentacle->interval8E6 = Script_GetPc() ? Script_GetValue() : 300;
    Centipede_ResetRouteHistory(actor);
    Tentacle_ReadPalette(actor, 0x22A, 0x192);
    Centipede_InitCollision(actor);
    Centipede_InitSegmentUpdateState(actor);
    MonsterInit_InstallPrimary(
        actor, Centipede_OnUpdate, Centipede_OnResolveVariantValue,
        CentipedeBody_OnIdleState, CentipedeBody_UpdateAttackWindup,
        CentipedeBody_ResetDirectionalPose, CentipedeBody_UpdateHiddenTransition,
        CentipedeBody_ConvergeReactionMotion, CentipedeBody_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, 0, SceneActorReaction_ResolveDamage, sub_080ABB18);
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
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    Centipede_SelectWanderRoute(actor);
    MonsterInit_SetState(actor, 0, 0, 2);
    MonsterInit_SetControl(actor, actor->base.onAlternateEntry, 1);
    SceneActorBehavior_InstallCallbackTables(actor);
    CentipedeBody_InitBehaviorTable(actor);
    Centipede_InitStateHandlers(actor);
    CentipedeBody_InitSegmentAnimations(actor, 0xD636, 0xC102, MonsterInit_GetDirection(actor));
    Centipede_ResetSegmentPositions(tentacle, &actor->base.body.position);
    Centipede_InitSegmentColliders(actor);
    Centipede_SelectWanderRoute(actor);
    Centipede_ActivateMarkedSegment(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Tentacle_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
