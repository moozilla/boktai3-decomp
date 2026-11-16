#include "scene_body_api.h"
#include "global.h"
#include "lun.h"
#include "scene_actor_state.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"
typedef void (*MonsterInitCommandHandler)(void *actor, const u16 *command);
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void Lun_InitProfile(void *);
void Lun_OnUpdate(void);
u32 Lun_GetRouteProgressRate(void *);
void Lun_TurnTowardSavedDirection(void);
void Lun_ReadSpawnScript(void *);
void Lun_UpdateDirectionalPoseNoSelect(void);
void Lun_UpdateDirectionalPose(void);
void Lun_ApproachMovementTarget(void);
void Lun_UpdateDefeat(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void Lun_PlayResponseAndResetState(void);
void SceneActorReaction_SinkAndRemove(void);
void Lun_ResetMovementState(void);
void Lun_EnterIdleOrCopyCell(void);
void Lun_OnTargetCheck(void);
void SceneActorScript_OnCommand(void *actor, const u16 *command);
void Lun_OnPlayerTargetInit(void);
void Lun_Destroy(void);
void AnimData_SelectPaletteBank(void *, s32);
void ActorContact_InitializeBodyAndSecondary(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void Lun_InitMotionPreset(void *);
void Lun_InitSecondaryActionTable(void *);
void SceneActorControl_SetStoredMotionCallbacks(void *);
void SceneActorControl_RestartPositionCurve(void *);
void ActorShadow_Disable(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

extern MonsterInitCommandHandler const gUnk_08605E78[];
extern MonsterInitCommandHandler const gUnk_086108B0[];
u32 Route_GetCurrentPointFacing(void *);
void Lun_InitReactionTable(void *);

#define SV(d) MonsterInit_ReadWord(d)

static inline void MonsterInit_InitializeBody(struct ChaseActor *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, r4v, r6v, r3v;
    s32 initialHealth, scriptVariant;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = Script_GetKeywordValue(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->base.tracking);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = MonsterInit_ReadWord(0x12C);
        r4v = MonsterInit_ReadWord(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->base.script1F0 = (u16)Script_GetKeywordValue(0x46, 0);
    arg0->base.scriptId = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        arg0->base.defaultHealth = MonsterInit_ReadHalfword(0);
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    initialHealth = MonsterInit_GetInitialHealth(arg0);
    r3v = r4v;
    arg0->base.motionFlags = 0;
    arg0->base.target = SceneActorControl_OnIdle;
    arg0->base.field2B2 = 1;
    arg0->base.stepDone = 0;
    arg0->base.health = MonsterInit_ClampHealth(initialHealth);
    arg0->base.attachmentTimer = 0;
    if (r3v <= 0)
        r3v = r6v;
    arg0->base.attachmentLimit = r6v;
    arg0->base.attachmentRestart = r3v;
    arg0->base.regionId = 0;

    if (Script_SeekToKeyword(0x58)) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    MonsterInit_WriteHalfword(&arg0->base.hudKind, Script_GetKeywordValue(0x4B, 2));
    scriptVariant = (s16)Script_GetKeywordValue(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    MonsterInit_WriteHalfword(&arg0->base.hudLabel, scriptVariant);
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->base.drawableKind ? 0 : arg0->base.drawable);
    SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);
}
static inline void Lun_ReadPalette(struct ChaseActor *actor, u16 base, u16 alternate)
{
    s32 r1v;
    actor->base.paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0x91);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->base.drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->base.drawable + 0x68) = gPaletteBankData + (r1v << 5);
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
static inline void Lun_InstallCompletionCallbacks(struct ChaseActor *actor)
{
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)Lun_OnPlayerTargetInit;
    actor->base.destroy = (SceneActorResultCallback)Lun_Destroy;
}

static inline void Lun_InstallRoutes(struct ChaseActor *actor)
{
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_08605E78);
    actor->base.routeReset = Lun_ResetMovementState;
    actor->base.reactionGate = Lun_EnterIdleOrCopyCell;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, Lun_OnTargetCheck);
}

static inline u32 Lun_ReadRoute(struct ChaseActor *actor, struct Lun *lun,
                                struct SceneActorRegistryRoute *route)
{
    struct ContactPosition point;
    route->activeBranch = 0;
    if (Script_SeekToKeyword('r')) {
        s32 pointId;
        route->routeId = MonsterInit_ReadWord(0);
        route->parameterB = 0;
        pointId = MonsterInit_ReadWord(0);
        route->pointId = pointId;
        route->currentPoint = pointId;
        route->branches[MonsterInit_RouteActiveBranch(route)] = route->routeId;
        route->branches[MonsterInit_RouteAlternateBranch(route)] =
            Script_GetPc() ? Script_GetValue() : MonsterInit_RouteId(route);
        Path_GetPointPositionById(&point, MonsterInit_RouteId(route),
                                  MonsterInit_RoutePointId(route));
        actor->base.terrainCell = MonsterInit_CellIndex(&point);
        point.y = actor->base.groundHeight =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &point);
        MonsterInit_BeginRoute(&actor->base.body, route);
        return 1;
    } else {
        route->routeId = 0;
        route->parameterB = 0;
        route->pointId = 0;
        route->currentPoint = 0;
        lun->route.branches[MonsterInit_RouteActiveBranch(route)] = 0;
        lun->route.branches[MonsterInit_RouteAlternateBranch(route)] = 0;
    }
    return 0;
}
static inline void Lun_ReadPosition(struct ChaseActor *actor, struct Lun *lun)
{
    if (Script_SeekToKeyword('p')) {
        s32 ground;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        actor->base.terrainCell = MonsterInit_CellIndex(&actor->base.body.position);
        ground = MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor),
                                           &actor->base.body.position);
        actor->base.groundHeight = ground;
        lun->ground3DC = ground;
        MonsterInit_DrawablePosition(actor);
    }
}

s32 Lun_Init(void *object)
{
    struct Lun *lun = object;
    struct ChaseActor *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 r1v;
    lun->flags3D4 = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x11);
    Lun_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD28E, 0x9607);
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
        res = ScriptProc_Start(0xCB05, 0xD28E);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    {
        struct SceneActorRegistryRoute *route = &lun->route;
        if (Lun_ReadRoute(actor, lun, route)) {
            MonsterInit_SnapToRoute(&actor->base.body.position, route);
            MonsterInit_DrawablePosition(actor);
            actor->base.body.direction = Route_GetCurrentPointFacing(route);
        }
    }
    Lun_ReadPosition(actor, lun);
    if (Script_SeekToKeyword('P')) {
        lun->parameter3D8 = MonsterInit_ReadByte(0);
        lun->parameter3D9 = MonsterInit_ReadByte(0);
    }
    lun->field442 = Script_GetKeywordValue('l', 0);
    Lun_ReadPalette(actor, 0x8C, 0x192);

    ActorContact_InitializeBodyAndSecondary(actor);
    MonsterInit_InstallPrimary(actor, Lun_OnUpdate, Lun_GetRouteProgressRate,
                               Lun_TurnTowardSavedDirection, Lun_UpdateDirectionalPoseNoSelect,
                               Lun_UpdateDirectionalPose, 0, Lun_ApproachMovementTarget,
                               Lun_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
                               SceneActorReaction_ResolveDamage, Lun_PlayResponseAndResetState);
    Lun_InstallRoutes(actor);
    MonsterInit_SetMotionFlags(actor, 0x60);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x02100400);
    if (MonsterInit_GetPaletteOffset(actor) != 0)
        MonsterInit_SetStatusFlags(actor, 8);
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
    SceneActorBehavior_InstallCallbackTables(actor);
    Lun_InitMotionPreset(actor);
    Lun_InitSecondaryActionTable(actor);
    Lun_InitReactionTable(actor);
    actor->base.field2E8 = 2;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Lun_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
