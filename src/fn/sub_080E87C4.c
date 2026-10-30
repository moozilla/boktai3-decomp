#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"

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

#define SK_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void CpuSet(const void *, void *, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorReaction_OnEnemyDefault(void);
u8 sub_0807F81C(void *);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorReaction_SinkAndRemove(void);
void sub_081363F4(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
void SceneActorReaction_BeginFadedState(void);
void SceneActorReaction_BeginDefeatBurst(void);
void sub_0807DB34(void);
void SceneActorReaction_RestoreAfterTurns(void);
void sub_0807FCA8(void);
void SceneActorReaction_TurnBackToRoute(void);
void sub_0807F130(void);
void SceneActorReaction_UpdateAnimationGate(void);
void sub_0808170C(void);
void SceneActorReaction_ResumeAfterAnimation(void);
void SceneActorReaction_TryResumeRoute(void);
void SceneActorReaction_BeginReactionSound(void);
void SceneActorScript_OnCommand(void);
void SceneActorControl_OnIdle(void);
void sub_08080BE0(void);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void SceneActorBehavior_InitializeShadow(void *);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);

#define SV(d) MonsterInit_ReadWord(d)

struct MimicInit32Block {
    u32 w[8];
};

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void CpuSet(const void *, void *, u32);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void sub_08080BE0(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorReaction_BeginFadedState(void);
void SceneActorReaction_BeginDefeatBurst(void);
void sub_0807DB34(void);
void SceneActorReaction_RestoreAfterTurns(void);
void sub_0807FCA8(void);
void SceneActorReaction_TurnBackToRoute(void);
void sub_0807F130(void);
void SceneActorReaction_UpdateAnimationGate(void);
void sub_0808170C(void);
void SceneActorReaction_ResumeAfterAnimation(void);
void SceneActorReaction_TryResumeRoute(void);
void SceneActorReaction_BeginReactionSound(void);
void SceneActorReaction_OnEnemyDefault(void);
void sub_081363F4(void);
void SceneActorScript_OnCommand(void);
void SceneActorControl_OnIdle(void);
extern const u8 gUnk_086108B0[];
extern const u8 gUnk_08606400[];
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
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
void *Region_FindContaining(void *);

extern u8 *gGameState;

static inline u16 MimicInit_PositionX(const u16 *pos)
{
    return pos[0];
}
static inline u16 MimicInit_PositionZ(const u16 *pos)
{
    return pos[2];
}
static inline s32 MimicInit_Cell(u16 *pos)
{
    s32 x = (s16)MimicInit_PositionX(pos) >> 8;
    s32 z = (s16)MimicInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct EngineMap *)gTerrainMap)->rows[z] + x;
}
static inline u32 MimicInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}

static inline u32 MimicInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}

static inline u32 MimicInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 attributes, shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct EngineMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    shape = MimicInit_TerrainShape(p);
    height = MimicInit_TerrainBaseHeight(p);
    switch (shape) {
    case 1: height -= ((u8 *)pos)[4]; break;
    case 2: height -= ((u8 *)pos)[0]; break;
    }
    return height;
}

struct MimicInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct ContactPosition position;
    u8 unknown64[0x34];
};
struct MimicInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct MimicInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct MimicInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct MimicInitRoutePoint { s16 x, z; };
struct MimicInitRoute {
    u32 unknown0, unknown4;
    struct MimicInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct MimicInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct MimicInitGate gate;
    struct MimicInitRange range;
    u8 unknown42C[0x10];
    void * field43C;
    u32 field440;
    void * field444;
    struct MimicInitRoute route;
    u8 unknown45C[0x15A];
    s16 field5B6;
    u8 unknown5B8[0x14];
    u16 field5CC;
    u8 unknown5CE[0x2];
    u8 * field5D0;
    u8 * field5D4;
    u8 unknown5D8[0x4];
    void * field5DC;
    void * field5E0;
    void * field5E4;
    u32 field5E8;
    void * field5EC;
    void * field5F0;
    void * field5F4;
    void * field5F8;
    void * field5FC;
    void * field600;
    u32 field604;
    u32 field608;
    void * field60C;
    void * field610;
    void * field614;
    void * field618;
    u32 field61C;
    u32 field620;};
struct MimicInitTargeting {
    u32 fieldDC;
    u8 unknownE0[0xA];
    u8 fieldEA;
    u8 fieldEB;
    u16 fieldEC;
    u16 fieldEE;
    u8 unknownF0[0x4];
    u32 fieldF4;
    u32 fieldF8;
    u32 fieldFC;
};
struct MimicInitSavedControl {
    u32 field118;
    u32 field11C;
    u8 field120;
    u8 field121;
    u8 field122;
    u8 field123;
    void *field124;
    u8 field128;
    u8 unknown129[0x3];
    u16 field12C;
    u16 field12E;
    u32 field130;
};
struct MimicInitActor {
    u8 unknown0[0x4];
    u8 * drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct MimicInitBody body;
    u8 category;
    u8 unknown99[0x1];
    u16 field9A;
    s16 field9C;
    u8 field9E;
    u8 field9F;
    u8 fieldA0;
    u8 fieldA1;
    u16 fieldA2;
    u16 fieldA4;
    u16 fieldA6;
    u16 fieldA8;
    u16 fieldAA;
    u32 fieldAC;
    u16 fieldB0;
    u16 fieldB2;
    struct MimicInitAttacks attacks;
    struct MimicInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct MimicInitSavedControl saved;
    u8 unknown134[0x6];
    u16 field13A;
    u8 unknown13C[0xA0];
    u16 field1DC, field1DE;
    u16 field1E0;
    u16 field1E2;
    u16 field1E4;
    u16 field1E6;
    u8 unknown1E8[0x4];
    u32 field1EC;
    u32 field1F0;
    u8 unknown1F4[0x28];
    void * field21C;
    void * field220;
    void * field224;
    void * field228;
    void * field22C;
    void * field230;
    void * field234;
    void * field238;
    void * field23C;
    u32 field240;
    u8 unknown244[0x4];
    void *field248;
    void * field24C;
    const void * field250;
    u8 unknown254[0x4];
    void * field258;
    void * field25C;
    u32 (*field260)(void *);
    void * field264;
    void * field268;
    void * field26C;
    void *field270;
    void * field274;
    void * field278;
    u8 unknown27C[0x2C];
    u8 field2A8;
    u8 field2A9;
    u8 field2AA;
    u8 unknown2AB[0x1];
    u32 field2AC;
    u8 field2B0;
    u8 field2B1;
    u8 field2B2;
    u8 unknown2B3[0x1];
    u8 field2B4;
    u8 field2B5;
    u8 unknown2B6[0x1];
    u8 field2B7;
    u8 unknown2B8[0x6];
    u16 field2BE;
    u16 field2C0;
    u16 field2C2;
    u8 unknown2C4[0x4];
    u16 field2C8;
    u8 unknown2CA[0x2];
    u32 field2CC;
    u32 field2D0;
    u16 field2D4;
    u8 unknown2D6[0x12];
    u16 field2E8;
    u16 field2EA;
    u8 unknown2EC[0x8];
    u16 field2F4;
    u16 field2F6;
    u8 unknown2F8[2];
    u16 field2FA;
    u8 unknown2FC[0x8];
    u32 field304;
    u8 unknown308[0xCC];
    struct MimicInitGroup group;
    u8 unknown624[0x2C];
    struct ContactPosition origin650;
    u16 originCell658;
    u8 unknown65A[4];
    u8 parameter65E;
    u8 unknown65F;
    s32 halfHealth660;
};

static inline u8 MimicInit_GetDrawableKind(struct MimicInitActor *actor)
{
    return actor->drawableKind;
}
static inline u8 MimicInit_GetCategory(void *object)
{
    struct MimicInitActor *actor = object;
    return actor->category;
}
static inline s32 MimicInit_GetHealth(struct MimicInitActor *actor)
{
    return actor->field9C;
}
static inline u16 MimicInit_GetInitialHealth(struct MimicInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 MimicInit_GetPaletteBase(struct MimicInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 MimicInit_GetPaletteOffset(struct MimicInitActor *actor)
{
    return actor->field1E6;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
u8 sub_0807F81C(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline u16 MimicInit_GetMotionFlags(struct MimicInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 MimicInit_GetHeightOffset(struct MimicInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 MimicInit_GetBehaviorFlags(struct MimicInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 MimicInit_GetStatusFlags(struct MimicInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 MimicInit_GetState(struct MimicInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 MimicInit_GetPhase(struct MimicInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 MimicInit_GetBehavior(struct MimicInitActor *actor)
{
    return actor->field2AA;
}
static inline void * MimicInit_GetCurrentControl(struct MimicInitActor *actor)
{
    return actor->field21C;
}
static inline u8 MimicInit_GetControlKind(struct MimicInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 MimicInit_GetDirection(struct MimicInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 MimicInit_GetTerrainCell(struct MimicInitActor *actor)
{
    return actor->field2F4;
}
static inline u32 MimicInit_GetTargetingFlags(struct MimicInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct MimicInitSavedControl *MimicInit_SavedControlOf(struct MimicInitActor *actor)
{
    struct MimicInitSavedControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *MimicInit_BodyPosition(void *object)
{
    struct MimicInitBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void MimicInit_SaveControl(struct MimicInitSavedControl *saved, struct MimicInitActor *actor)
{
    saved->field12E = MimicInit_GetMotionFlags(actor);
    saved->field12C = MimicInit_GetHeightOffset(actor);
    saved->field118 = MimicInit_GetBehaviorFlags(actor);
    saved->field11C = MimicInit_GetStatusFlags(actor);
    saved->field120 = MimicInit_GetDirection(actor);
    saved->field121 = MimicInit_GetState(actor);
    saved->field122 = MimicInit_GetPhase(actor);
    saved->field123 = MimicInit_GetBehavior(actor);
    saved->field124 = MimicInit_GetCurrentControl(actor);
    saved->field128 = MimicInit_GetControlKind(actor);
}
struct MimicInitCollider { u8 header[6]; u16 flags; };
static inline void MimicInit_ClearColliderFlags(struct MimicInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 MimicInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void MimicInit_SetBehaviorFlags(struct MimicInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void MimicInit_SetStatusFlags(struct MimicInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline void MimicInit_SetMotionFlags(struct MimicInitActor *actor, u16 mask)
{
    actor->field2C0 |= mask;
}
static inline u8 MimicInit_TestMotionFlags(struct MimicInitActor *actor, u32 mask)
{
    if (MimicInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 MimicInit_TestTargetingFlags(struct MimicInitTargeting *target, u32 mask)
{
    if (MimicInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void MimicInit_ClearControlFlags(struct MimicInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 MimicInit_IsInCurrentRegion(struct MimicInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *MimicInit_GetStandingControl(struct MimicInitActor *actor)
{
    return actor->field22C;
}

static inline void MimicInit_SetControl(struct MimicInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    MimicInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void MimicInit_SetState(struct MimicInitActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}

static inline void MimicInit_InitializeGate(struct MimicInitGate *gate, u32 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;

}

static inline void MimicInit_ReadTargeting(struct MimicInitTargeting *target)
{
    s32 r4v, r2v, r1v;
    if (Script_SeekToKeyword(0x65)) {
        r4v = SV(0x30);
        r2v = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        r1v = r4v;
    } else {
        r4v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    target->fieldDC = 0;
    target->fieldEC = r4v;
    target->fieldEE = r1v;
    target->fieldF8 = r4v * r4v;
    target->fieldFC = r1v * r1v;
    target->fieldEA = r2v;
    if (MimicInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);

}

static inline u16 MimicInit_RangeRadius(struct MimicInitRange *range)
{
    return range->field422;
}
static inline void MimicInit_ReadRange(struct MimicInitRange *range)
{
    s32 r4v;
    if (Script_SeekToKeyword(0x6E)) {
        r4v = SV(0);
        range->field420 = SV(0);
    } else {
        r4v = 0;
        range->field420 = r4v;
    }
    range->field422 = r4v;
    range->field424 = MimicInit_RangeRadius(range) * MimicInit_RangeRadius(range);
    range->field42A = 0;

}

static inline u32 MimicInit_RoutePointX(struct MimicInitRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 MimicInit_RoutePointZ(struct MimicInitRoute *route)
{
    return (u16)route->point->z;
}

static inline void MimicInit_SnapToRoute(struct ContactPosition *position, struct MimicInitRoute *route)
{
    position->x = MimicInit_RoutePointX(route);
    position->z = MimicInit_RoutePointZ(route);
    position->y = MimicInit_Height(MimicInit_Cell(position), (u16 *)position);
}
static inline void MimicInit_DrawablePosition(struct MimicInitActor *actor)
{
    if (MimicInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 MimicInit_RouteId(struct MimicInitRoute *route)
{
    return route->routeId;
}
static inline u8 MimicInit_RouteParameter(struct MimicInitRoute *route)
{
    return route->parameterB;
}
static inline u8 MimicInit_RoutePointId(struct MimicInitRoute *route)
{
    return route->pointId;
}
static inline u8 MimicInit_RouteActiveBranch(struct MimicInitRoute *route)
{
    return route->activeBranch;
}

static inline s32 MimicInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void MimicInit_InstallIdle(struct MimicInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void MimicInit_InitializeHealth(struct MimicInitActor *arg0,
    u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    MimicInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = MimicInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;

}

static inline void MimicInit_SetInitialHealth(struct MimicInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 MimicInit_RouteAlternateBranch(struct MimicInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void MimicInit_BeginRoute(struct MimicInitBody *body, struct MimicInitRoute *route)
{
    SceneBody_StartRoute(body, route, MimicInit_RouteId(route), MimicInit_RouteParameter(route), MimicInit_RoutePointId(route));
}

static inline void MimicInit_SetFirstAttack(struct MimicInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 MimicInit_ReadRoute(struct MimicInitActor *arg0, struct MimicInitGroup *group, struct MimicInitRoute *rt)
{
    struct ContactPosition point;
    s32 r1v;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = SV(0);
        rt->parameterB = 0;
        r1v = SV(0);
        rt->pointId = r1v;
        rt->currentPoint = r1v;
        rt->branches[MimicInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[MimicInit_RouteAlternateBranch(rt)] = Script_GetPc() ? Script_GetValue() : MimicInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, MimicInit_RouteId(rt), MimicInit_RoutePointId(rt));
        arg0->field2F4 = MimicInit_Cell(&point);
        point.y = arg0->field2BE = MimicInit_Height(MimicInit_GetTerrainCell(arg0), (u16 *)&point);
        MimicInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[MimicInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[MimicInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void MimicInit_InitializeBody(void *object, u32 *fill)
{
    struct MimicInitActor *arg0 = object;
    struct MimicInitGroup *group = &arg0->group;
    struct MimicInitBody *body = &arg0->body;
    struct ContactPosition origin;
    struct ContactPosition point;
    s32 mode, i, r4v, r6v, r5v, r2v, r1v;
    u16 initialHealth;
    struct MimicInitRoute *rt;
    arg0->field2CC = 0;
    arg0->field2D0 = 0;
    arg0->field2B4 = 0;
    arg0->field2E8 = 8;
    arg0->field2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        i = 0;
        while (1) {
            MimicInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = MonsterInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = SV(0);
            arg0->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        r5v = SV(0x12C);
        r6v = SV(0x12C);
        r2v = SV(0x12C);
    } else {
        r5v = 0;
        r6v = 0;
        r2v = 0;
    }
    MimicInit_InitializeGate(&group->gate, r2v);
    MimicInit_ReadRange(&group->range);
    MimicInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        MimicInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MimicInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->field5B6 = MimicInit_Half(MimicInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MimicInit_InitializeHealth(arg0, MimicInit_GetInitialHealth(arg0),
        r5v, r6v);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = SV(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    r1v = (s16)Script_GetKeywordValue(0x56, -1);
    if (r1v < 0)
        r1v = 0xFFFF;
    arg0->field9A = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        MimicInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MimicInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (MimicInit_ReadRoute(arg0, group, rt)) {
        MimicInit_SnapToRoute(&arg0->body.position, rt);
        MimicInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MimicInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, MimicInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MimicInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct MimicInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MimicInit_SetWidgetScriptPalette(struct MimicInitWidget *widget, s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void MimicInit_SetMotionControl(struct MimicInitActor *actor, void *callback, u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void MimicInit_ClearMotion(void *object)
{
    struct MimicInitActor *actor = object;
    u32 zero = 0;
    MimicInit_SetMotionControl(actor, 0, 0);
    actor->field114 = zero;
}
static inline void MimicInit_ReadPalette(struct MimicInitActor *actor, u32 base, u32 alternate)
{
    s32 r1v;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = MimicInit_GetPaletteOffset(actor) + base;
    r1v = MimicInit_GetPaletteBase(actor) + MimicInit_GetPaletteOffset(actor);
    if (MimicInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MimicInit_GetDrawableKind(actor) == 1)
        MimicInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20), SceneActorScript_GetManagerField62());
}

static inline void MimicInit_InstallPrimary(struct MimicInitActor *arg0,
    void *cb0,
    u32 (*cb1)(void *),
    void *cb2,
    void *cb3,
    void *cb4, void *hidden,
    void *cb5,
    void *cb6, void *defaultReaction, void *cb270,
    void *targetQuery, void *damage, void *contact)
{
    MimicInit_SetBehaviorFlags(arg0, 0x2000);
    arg0->field274 = cb0;
    arg0->field260 = cb1;
    arg0->field224 = cb2;
    arg0->field228 = cb3;
    arg0->field22C = cb4;
    arg0->field230 = cb5;
    arg0->field234 = cb6;
    arg0->field220 = hidden;
    arg0->field278 = defaultReaction;
    arg0->field270 = cb270;
    arg0->field250 = gUnk_086108B0;
    arg0->field248 = 0;
    arg0->field26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->field264 = damage;
    arg0->field268 = contact;
    arg0->field240 = 0;
    MimicInit_ClearMotion(arg0);
}

static inline void MimicInit_InitializeGroup(struct MimicInitGroup *group, void *firstBounds,
    void *secondBounds, void *downed, void *returnToRoute, void *recover,
    void *animate, struct MimicInitBody *body)
{
    group->field5D0 = firstBounds;
    group->field5D4 = secondBounds;
    group->field5F4 = downed;
    group->field5F8 = recover;
    group->field5FC = SceneActorReaction_BeginFadedState;
    group->field600 = SceneActorReaction_BeginDefeatBurst;
    group->field43C = returnToRoute;
    group->field440 = 0;
    group->range.field42B = 1;
    group->field618 = sub_0807DB34;
    group->field608 = 0;
    group->field61C = 0;
    group->field620 = 0;
    group->field5F0 = SceneActorReaction_RestoreAfterTurns;
    group->field60C = sub_0807FCA8;
    group->field604 = (u32)animate;
    group->field614 = SceneActorReaction_TurnBackToRoute;
    group->field5E8 = 0;
    group->field444 = sub_0807F130;
    group->gate.field41C = SceneActorReaction_UpdateAnimationGate;
    group->gate.field41A = 0x28;
    group->field5DC = sub_0808170C;
    group->field5E0 = SceneActorReaction_ResumeAfterAnimation;
    group->field5E4 = SceneActorReaction_TryResumeRoute;
    group->field610 = SceneActorReaction_BeginReactionSound;
    ProximityVolume_TranslateBounds(firstBounds, MimicInit_BodyPosition(body));
}

static inline void MimicInit_InitializeRegion(struct MimicInitActor *arg0)
{
    if (MimicInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);

}

static inline void MimicInit_InstallCompletionCallbacks(struct MimicInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Mimic_OnUpdate;
    arg0->field25C = Mimic_ReleaseResources;

}

static inline u8 *MimicInit_Drawable(struct MimicInitActor *actor)
{
    return actor->drawable;
}

static inline void MimicInit_SetCategory(struct MimicInitActor *actor, u8 category)
{
    actor->category = category;
}

static inline void MimicInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void MimicInit_SetPaletteBank(struct MimicInitActor *actor, u16 palette)
{
    actor->field1DE = palette;
}
static inline void MimicInit_InstallRoutes(struct MimicInitActor *actor)
{
    actor->field248 = Mimic_ResetPursuitDrawable;
    actor->field23C = SceneActor_PrepareStationaryReaction;
}
static inline void MimicInit_ClearReceiver(struct MimicInitActor *actor)
{
    actor->field304 = 0;
}
static inline void MimicInit_InitializeTerrainCell(struct MimicInitActor *actor, s32 cell)
{
    actor->field2F4 = cell;
    actor->field2F6 = cell;
}

static inline void MimicInit_InitializeWidget(u8 *drawable, u16 script, u16 resource, struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct MimicInit32Block *)drawable = *(struct MimicInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

s32 Mimic_Init(void *object)
{
    struct MimicInitActor *arg0 = object;
    struct MimicInitActor *actor = object;
    struct MimicInitGroup *group = &arg0->group;
    struct MimicInitBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 lift;

    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MimicInit_SetCategory(actor, 10);
    Mimic_InitProfile(actor);
    if (MimicInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MimicInit_Drawable(actor);
        MimicInit_InitializeSprite(heap, 0x3876, 0x9AF2);
        if (MimicInit_GetCategory(actor) == 0x11 || MimicInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MimicInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MimicInit_InitializeWidget(heap, 0xCB05, 0x3876, &tmp);
        actor->field2B7 = 0xFF;
    }
    MimicInit_InitializeBody(arg0, &zero);
    arg0->halfHealth660 = MimicInit_GetHealth(actor) >> 1;
    actor->body.direction = Script_GetKeywordValue('d', 0);
    arg0->parameter65E = Script_GetKeywordValue('s', 10);
    if (Script_SeekToKeyword('p')) {
        s32 cell;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        MimicInit_InitializeTerrainCell(actor, MimicInit_Cell((u16 *)&actor->body.position));
        actor->field2BE = MimicInit_Height(MimicInit_GetTerrainCell(actor), (u16 *)&actor->body.position);
        actor->field2EA = 0;
        actor->body.position.y = actor->field2BE;
        MimicInit_DrawablePosition(actor);
    }
    arg0->origin650 = actor->body.position;
    arg0->originCell658 = actor->field2F4;
    MimicInit_ReadPalette(actor, 0x304, 0x192);
    MimicInit_SetPaletteBank(actor, 0x306);
    SceneActor_InitializeReactionColliders(actor);
    MimicInit_InstallPrimary(actor, Mimic_UpdateBehavior, 0, 0,
        SceneActor_EnterFlaggedEntryPhase, Mimic_OnAnimationStateChanged, 0,
        Mimic_ConvergeAndTurn, Mimic_UpdateDefeatBlink, SceneActorReaction_OnEnemyDefault,
        SceneActor_AcquireCollisionTarget, SceneActorTarget_ResolvePendingObject,
        sub_081363F4, Mimic_StartHitReaction);
    MimicInit_InitializeGroup(group, (u8 *)arg0 + 0x624, (u8 *)arg0 + 0x648,
        0, 0, Mimic_OnBehaviorComplete, 0, body);
    MimicInit_InstallRoutes(actor);
    MimicInit_SetBehaviorFlags(actor, 0x400200);
    MimicInit_SetStatusFlags(actor, 8);
    MimicInit_SetMotionFlags(actor, 0x40);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MimicInit_TestMotionFlags(actor, 4)) {
        MimicInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MimicInit_SetState(actor, 0, 0, 0);
        MimicInit_SetControl(actor, MimicInit_GetStandingControl(actor), 0);
    }
    MimicInit_InitializeRegion(actor);
    if (MimicInit_TestMotionFlags(actor, 0x40))
        MimicInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MimicInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MimicInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MimicInit_ClearReceiver(actor);
    lift = Script_GetKeywordValue('f', 0);
    if (lift) {
        actor->field2EA = lift;
        MimicInit_SetControl(actor, SceneActor_RestoreSavedTargetOnCompletion, 0x26);
        MimicInit_SetState(actor, 3, 0, 0);
    }
    SceneActorBehavior_InstallCallbackTables(actor);
    Mimic_InitBehaviorTable(actor);
    Mimic_InitActionTable(actor);
    actor->field2E8 = 0;
    MimicInit_SaveControl(MimicInit_SavedControlOf(actor), actor);
    MimicInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
