#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
u32 SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorScript_OnCommand(void);
void AnimData_SelectPaletteBank(void *, s32);
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

static inline void SkeletonInit_ReadWidgetRelativePalette(struct DisplayKeyframeState *widget)
{
    widget->paletteIndex = Script_GetPc() ? Script_GetValue() + 0x12B : 0x12B;
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

static inline void SkeletonInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Skeleton_UpdateBehaviorCallbacks;
    arg0->destroyCallback = Skeleton_Destroy;
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
        MonsterInit_InitializeSprite(heap, 0xAE52, 0);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
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

    MonsterInit_InitializePursuitBody(arg0, &zero);
    SceneActorReaction_ParseOptions(actor);
    SceneActorScript_ReadPParameters(actor);
    SceneActorScript_ReadCallScriptConfig(actor);

    MonsterInit_ReadPalette(actor, 0x13C, 0x146);
    SceneActorControl_InitBehaviorColliders(actor);

    MonsterInit_InstallSequenceCallbacks(
        actor, Skeleton_Update, SceneActorReaction_SelectTileParameter, sub_080BAE04,
        Skeleton_UpdateInactiveState, Skeleton_UpdateIdleState,
        TentacleBody_UpdateReactionTransition, Skeleton_ConvergeDisplacedPosition,
        Skeleton_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_0807F81C,
        SceneActorTarget_ResolvePendingObjectSkeleton, SceneActorReaction_ResolvePlacementDamage,
        SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, (u8 *)arg0 + 0x71C, (u8 *)arg0 + 0x740, Skeleton_UpdateDownedState,
        SceneActorReaction_ReturnToRouteEntry, SceneActorReaction_BeginTargetRecovery, 0, 1, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MonsterInit_SetRouteReset(actor, Skeleton_ResetRouteReaction);
    group->responseCallback = 0;
    MonsterInit_SetReactionGate(actor, SceneActorReaction_UpdateGate);
    group->field5EC = Skeleton_RestorePursuitRoute;
    SkeletonInit_InstallContactDefaults(actor);
    actor->field2E8 = 0;
    ((struct SkeletonInstanceState *)arg0)->field718 = (s16)MonsterInit_GetHealth(actor) >> 1;
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
