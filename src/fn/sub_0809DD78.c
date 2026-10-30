#include "global.h"
#include "scene_actor_state.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_group.h"
typedef void (*MonsterInitCommandHandler)(void *actor, const u16 *command);
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
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
void SceneActorScript_OnCommand(void);
void Lun_OnPlayerTargetInit(void);
void Lun_Destroy(void);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 SceneActorScript_GetManagerField62(void);
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
struct MonsterInit32Block {
    u32 w[8];
};
struct LunActor {
    u8 unknown0[0x3D4];
    u32 flags;
    u8 parameter3D8, parameter3D9;
    u8 unknown3DA[2];
    u16 ground3DC;
    u8 unknown3DE[0x4E];
    struct SceneActorRegistryRoute route;
    u8 unknown440[2];
    u16 field442;
};
u32 Route_GetCurrentPointFacing(void *);
void Lun_InitReactionTable(void *);

#define SV(d) MonsterInit_ReadWord(d)
struct MonsterInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MonsterInit_SetWidgetPalette(struct MonsterInitWidget *widget, u32 palette)
{
    widget->paletteBank = palette;
}

static inline void MonsterInit_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, r4v, r6v, r3v;
    s32 initialHealth, scriptVariant;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = Script_GetKeywordValue(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->tracking);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = MonsterInit_ReadWord(0x12C);
        r4v = MonsterInit_ReadWord(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = (u16)Script_GetKeywordValue(0x46, 0);
    arg0->field1EC = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        arg0->defaultHealth = MonsterInit_ReadHalfword(0);
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    initialHealth = MonsterInit_GetInitialHealth(arg0);
    r3v = r4v;
    arg0->flags2D4 = 0;
    arg0->target = SceneActorControl_OnIdle;
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = MonsterInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (r3v <= 0)
        r3v = r6v;
    arg0->attachmentLimit = r6v;
    arg0->fieldA8 = r3v;
    arg0->regionId = 0;

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    MonsterInit_WriteHalfword(&arg0->fieldB2, Script_GetKeywordValue(0x4B, 2));
    scriptVariant = (s16)Script_GetKeywordValue(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    MonsterInit_WriteHalfword(&arg0->field9A, scriptVariant);
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->drawableKind ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);
}
static inline void Lun_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                   u16 alternate)
{
    s32 r1v;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    MonsterInit_SetCurrentPalette(actor, MonsterInit_GetPaletteOffset(actor) + 0x91);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
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
static inline void Lun_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = Lun_OnPlayerTargetInit;
    actor->destroyCallback = Lun_Destroy;
}

static inline void MonsterInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void Lun_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_08605E78);
    actor->callback248 = Lun_ResetMovementState;
    actor->field23C = Lun_EnterIdleOrCopyCell;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, Lun_OnTargetCheck);
}

static inline void Lun_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline u32 Lun_ReadRoute(struct SceneActorRegistryResetView *actor, struct LunActor *lun,
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
        actor->terrainCell = MonsterInit_CellIndex(&point);
        point.y = actor->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &point);
        MonsterInit_BeginRoute(&actor->body, route);
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
static inline void Lun_ReadPosition(struct SceneActorRegistryResetView *actor, struct LunActor *lun)
{
    if (Script_SeekToKeyword('p')) {
        s32 ground;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        actor->terrainCell = MonsterInit_CellIndex(&actor->body.position);
        ground =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &actor->body.position);
        actor->groundHeight2BE = ground;
        lun->ground3DC = ground;
        Lun_DrawablePosition(actor);
    }
}

s32 Lun_Init(void *object)
{
    struct LunActor *lun = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 r1v;
    lun->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x11);
    Lun_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD28E, 0x9607);
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
        res = ScriptProc_Start(0xCB05, 0xD28E);
        *(struct MonsterInit32Block *)heap = *(struct MonsterInit32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    {
        struct SceneActorRegistryRoute *route = &lun->route;
        if (Lun_ReadRoute(actor, lun, route)) {
            MonsterInit_SnapToRoute(&actor->body.position, route);
            Lun_DrawablePosition(actor);
            actor->body.direction = Route_GetCurrentPointFacing(route);
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
        MonsterInit_ClearColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Lun_InitMotionPreset(actor);
    Lun_InitSecondaryActionTable(actor);
    Lun_InitReactionTable(actor);
    actor->field2E8 = 2;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Lun_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
