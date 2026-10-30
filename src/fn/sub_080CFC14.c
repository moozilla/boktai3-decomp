#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"

struct GolemResourceBlock {
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
u32 SceneActorReaction_OnEnemyDefault(void);
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

static inline u16 GolemInit_PositionX(const struct ContactPosition *pos)
{
    return pos->x;
}
static inline u16 GolemInit_PositionZ(const struct ContactPosition *pos)
{
    return pos->z;
}
static inline u32 GolemInit_Cell(struct ContactPosition *pos)
{
    s32 x = (s16)GolemInit_PositionX(pos) >> 8;
    s32 z = (s16)GolemInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct EngineMap *)gTerrainMap)->rows[z] + x;
}
static inline u8 GolemInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}
static inline u32 GolemInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}
static inline u32 GolemInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct EngineMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    shape = GolemInit_TerrainShape(p);
    height = GolemInit_TerrainBaseHeight(p);
    switch (shape) {
        case 1:
            height -= ((u8 *)pos)[4];
            break;
        case 2:
            height -= ((u8 *)pos)[0];
            break;
    }
    return height;
}

struct GolemInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct ContactPosition position;
    u8 unknown64[0x34];
};
struct GolemInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct GolemInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct GolemInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct GolemInitRoutePoint {
    s16 x, z;
};
struct GolemInitRoute {
    u32 unknown0, unknown4;
    struct GolemInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct GolemInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct GolemInitGate gate;
    struct GolemInitRange range;
    u8 unknown42C[0x10];
    void *field43C;
    u32 field440;
    void *field444;
    struct GolemInitRoute route;
    u8 unknown45C[0x15A];
    s16 field5B6;
    u8 unknown5B8[0x14];
    u16 field5CC;
    u8 unknown5CE[0x2];
    u8 *field5D0;
    u8 *field5D4;
    u8 unknown5D8[0x4];
    void *field5DC;
    void *field5E0;
    void *field5E4;
    u32 field5E8;
    void *field5EC;
    void *field5F0;
    void *field5F4;
    void *field5F8;
    void *field5FC;
    void *field600;
    u32 field604;
    u32 field608;
    void *field60C;
    void *field610;
    void *field614;
    void *field618;
    u32 field61C;
    u32 field620;
};
struct GolemInitTargeting {
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
struct GolemInitSavedControl {
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
struct GolemInitContext {
    u8 unknown0[0x62C];
    u32 rangeSquared62C;
};
struct GolemInitActor {
    u8 unknown0[0x4];
    u8 *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct GolemInitBody body;
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
    struct GolemInitAttacks attacks;
    struct GolemInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct GolemInitSavedControl saved;
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
    void *field21C;
    void (*field220)(void *);
    void (*field224)(void *);
    void (*field228)(void *);
    void (*field22C)(void *);
    void (*field230)(void *);
    void (*field234)(void *);
    void *field238;
    void *field23C;
    u32 field240;
    u8 unknown244[0x4];
    void *field248;
    void *field24C;
    const void *field250;
    u8 unknown254[0x4];
    void *field258;
    void *field25C;
    u16 (*field260)(void *);
    void *field264;
    void *field268;
    void *field26C;
    u8 (*field270)(void *);
    s32 (*field274)(void *);
    u32 (*field278)(void);
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
    u8 unknown2F6[0x4];
    u16 field2FA;
    u8 unknown2FC[0x8];
    u32 field304;
    u8 unknown308[0xC8];
    struct GolemInitContext *context;
    struct GolemInitGroup group;
    u8 field624;
    u8 unknown625[3];
    u32 field628;
    u8 unknown62C[0x54];
    u16 field680, field682, field684, field686;
    u16 unknown688;
    u16 field68A;
    u8 unknown68C;
    u8 field68D;
    u8 unknown68E[5];
    u8 field693;
};

static inline u8 *GolemInit_Drawable(struct GolemInitActor *actor)
{
    return actor->drawable;
}
static inline u8 GolemInit_GetDrawableKind(struct GolemInitActor *actor)
{
    return actor->drawableKind;
}
static inline void GolemInit_SetCategory(void *object, u8 category)
{
    struct GolemInitActor *actor = object;
    actor->category = category;
}
static inline u8 GolemInit_GetCategory(void *object)
{
    struct GolemInitActor *actor = object;
    return actor->category;
}

static inline u16 GolemInit_GetInitialHealth(struct GolemInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 GolemInit_GetPaletteBase(struct GolemInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 GolemInit_GetPaletteOffset(struct GolemInitActor *actor)
{
    return actor->field1E6;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_0807F81C(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline u32 GolemInit_GetMotionFlags(struct GolemInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 GolemInit_GetHeightOffset(struct GolemInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 GolemInit_GetBehaviorFlags(struct GolemInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 GolemInit_GetStatusFlags(struct GolemInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 GolemInit_GetState(struct GolemInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 GolemInit_GetPhase(struct GolemInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 GolemInit_GetBehavior(struct GolemInitActor *actor)
{
    return actor->field2AA;
}
static inline void *GolemInit_GetCurrentControl(struct GolemInitActor *actor)
{
    return actor->field21C;
}
static inline u8 GolemInit_GetControlKind(struct GolemInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 GolemInit_GetDirection(struct GolemInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 GolemInit_GetTerrainCell(struct GolemInitActor *actor)
{
    return actor->field2F4;
}
static inline u8 GolemInit_GetTargetingFlags(struct GolemInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct GolemInitSavedControl *GolemInit_SavedControlOf(struct GolemInitActor *actor)
{
    struct GolemInitSavedControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *GolemInit_BodyPosition(void *object)
{
    struct GolemInitBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void GolemInit_SaveControl(struct GolemInitSavedControl *saved,
                                         struct GolemInitActor *actor)
{
    saved->field12E = GolemInit_GetMotionFlags(actor);
    saved->field12C = GolemInit_GetHeightOffset(actor);
    saved->field118 = GolemInit_GetBehaviorFlags(actor);
    saved->field11C = GolemInit_GetStatusFlags(actor);
    saved->field120 = GolemInit_GetDirection(actor);
    saved->field121 = GolemInit_GetState(actor);
    saved->field122 = GolemInit_GetPhase(actor);
    saved->field123 = GolemInit_GetBehavior(actor);
    saved->field124 = GolemInit_GetCurrentControl(actor);
    saved->field128 = GolemInit_GetControlKind(actor);
}
struct GolemInitCollider {
    u8 header[6];
    u16 flags;
};
static inline void GolemInit_ClearColliderFlags(struct GolemInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 GolemInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void GolemInit_SetBehaviorFlags(struct GolemInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void GolemInit_SetStatusFlags(struct GolemInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline u8 GolemInit_TestMotionFlags(struct GolemInitActor *actor, u32 mask)
{
    if (GolemInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 GolemInit_TestTargetingFlags(struct GolemInitTargeting *target, u32 mask)
{
    if (GolemInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void GolemInit_SetTargetingFlags(struct GolemInitTargeting *target, u8 mask)
{
    mask |= target->fieldEB;
    target->fieldEB = mask;
}
static inline void GolemInit_ClearControlFlags(struct GolemInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 GolemInit_IsInCurrentRegion(struct GolemInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *GolemInit_GetStandingControl(struct GolemInitActor *actor)
{
    return actor->field22C;
}
static inline void GolemInit_SetControl(struct GolemInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    GolemInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void GolemInit_SetState(struct GolemInitActor *actor, u32 state, u32 phase,
                                      u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}
static inline void GolemInit_ResetFlags(void *object)
{
    struct GolemInitActor *actor = object;
    actor->field628 = 0;
}

static inline void GolemInit_InitializeGate(struct GolemInitGate *gate, u16 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;
}

static inline void GolemInit_ReadTargeting(struct GolemInitTargeting *target)
{
    s32 nearRadius, flags, farRadius;
    if (Script_SeekToKeyword(0x65)) {
        nearRadius = MonsterInit_ReadWord(0x30);
        flags = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        farRadius = nearRadius;
    } else {
        nearRadius = 0x30;
        farRadius = 0x30;
        flags = 0x40;
    }
    target->fieldDC = 0;
    target->fieldEC = nearRadius;
    target->fieldEE = farRadius;
    target->fieldF8 = nearRadius * nearRadius;
    target->fieldFC = farRadius * farRadius;
    target->fieldEA = flags;
    if (GolemInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);
}

static inline u16 GolemInit_RangeRadius(struct GolemInitRange *range)
{
    return range->field422;
}
static inline void GolemInit_ReadRange(struct GolemInitRange *range)
{
    s32 radius;
    if (Script_SeekToKeyword(0x6E)) {
        radius = MonsterInit_ReadWord(0);
        range->field420 = MonsterInit_ReadHalfword(0);
    } else {
        radius = 0;
        range->field420 = radius;
    }
    range->field422 = radius;
    range->field424 = GolemInit_RangeRadius(range) * GolemInit_RangeRadius(range);
    range->field42A = 0;
}

static inline u32 GolemInit_RoutePointX(struct GolemInitRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 GolemInit_RoutePointZ(struct GolemInitRoute *route)
{
    return (u16)route->point->z;
}
static inline void GolemInit_SnapToRoute(struct ContactPosition *position,
                                         struct GolemInitRoute *route)
{
    position->x = GolemInit_RoutePointX(route);
    position->z = GolemInit_RoutePointZ(route);
    position->y = GolemInit_Height(GolemInit_Cell(position), (u16 *)position);
}
static inline void GolemInit_DrawablePosition(struct GolemInitActor *actor)
{
    if (GolemInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void GolemInit_SetFirstAttack(struct GolemInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u8 GolemInit_RouteId(struct GolemInitRoute *route)
{
    return route->routeId;
}
static inline u8 GolemInit_RouteParameter(struct GolemInitRoute *route)
{
    return route->parameterB;
}
static inline u8 GolemInit_RoutePointId(struct GolemInitRoute *route)
{
    return route->pointId;
}
static inline u8 GolemInit_RouteActiveBranch(struct GolemInitRoute *route)
{
    return route->activeBranch;
}
static inline u32 GolemInit_RouteAlternateBranch(struct GolemInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void GolemInit_BeginRoute(struct GolemInitBody *body, struct GolemInitRoute *route)
{
    SceneBody_StartRoute(body, route, GolemInit_RouteId(route), GolemInit_RouteParameter(route),
                         GolemInit_RoutePointId(route));
}
static inline s32 GolemInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void GolemInit_InstallIdle(struct GolemInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void GolemInit_InitializeHealth(struct GolemInitActor *arg0, u16 initialHealth,
                                              s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    GolemInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = GolemInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;
}

static inline void GolemInit_SetInitialHealth(struct GolemInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 GolemInit_ReadRoute(struct GolemInitActor *arg0, struct GolemInitGroup *group,
                                      struct GolemInitRoute *rt)
{
    struct ContactPosition point;
    s32 pointId;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = MonsterInit_ReadByte(0);
        rt->parameterB = 0;
        pointId = MonsterInit_ReadWord(0);
        rt->pointId = pointId;
        rt->currentPoint = pointId;
        rt->branches[GolemInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[GolemInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : GolemInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, GolemInit_RouteId(rt), GolemInit_RoutePointId(rt));
        arg0->field2F4 = GolemInit_Cell(&point);
        point.y = arg0->field2BE = GolemInit_Height(GolemInit_GetTerrainCell(arg0), (u16 *)&point);
        GolemInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[GolemInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[GolemInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void GolemInit_InitializeBody(void *object, u32 *fill)
{
    struct GolemInitActor *arg0 = object;
    struct GolemInitGroup *group = &arg0->group;
    struct GolemInitBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, i, secondLimit, firstLimit, gateDuration, identity;
    struct GolemInitRoute *rt;
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
            GolemInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = MonsterInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = MonsterInit_ReadWord(0);
            arg0->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
        gateDuration = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    GolemInit_InitializeGate(&group->gate, gateDuration);
    GolemInit_ReadRange(&group->range);
    GolemInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        GolemInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        GolemInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->field5B6 = GolemInit_Half(GolemInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    GolemInit_InitializeHealth(arg0, GolemInit_GetInitialHealth(arg0), firstLimit, secondLimit);

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
        GolemInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        GolemInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (GolemInit_ReadRoute(arg0, group, rt)) {
        GolemInit_SnapToRoute(&arg0->body.position, rt);
        GolemInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (GolemInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, GolemInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (GolemInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct GolemInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void GolemInit_SetWidgetScriptPalette(struct GolemInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void GolemInit_SetMotionControl(struct GolemInitActor *actor, void *callback,
                                              u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void GolemInit_ClearMotion(void *object)
{
    struct GolemInitActor *actor = object;
    GolemInit_SetMotionControl(actor, 0, 0);
    actor->field114 = 0;
}
static inline void GolemInit_ReadPalette(struct GolemInitActor *actor, u32 base, u32 alternate)
{
    s32 palette;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = GolemInit_GetPaletteOffset(actor) + base;
    palette = GolemInit_GetPaletteBase(actor) + GolemInit_GetPaletteOffset(actor);
    if (GolemInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (GolemInit_GetDrawableKind(actor) == 1)
        GolemInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                         SceneActorScript_GetManagerField62());
}

static inline void GolemInit_InstallPrimary(struct GolemInitActor *arg0, s32 (*cb0)(void *),
                                            u16 (*cb1)(void *), void (*cb2)(void *),
                                            void (*cb3)(void *), void (*cb4)(void *),
                                            void (*hidden)(void *), void (*cb5)(void *),
                                            void (*cb6)(void *), u32 (*defaultReaction)(void),
                                            u8 (*cb270)(void *), void *targetQuery, void *damage,
                                            void *contact)
{
    GolemInit_SetBehaviorFlags(arg0, 0x2000);
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
    GolemInit_ClearMotion(arg0);
}

static inline void GolemInit_InitializeGroup(struct GolemInitGroup *group, void *firstBounds,
                                             void *secondBounds, void *downed, void *returnToRoute,
                                             void *recover, void *animate,
                                             struct GolemInitBody *body)
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
    ProximityVolume_TranslateBounds(firstBounds, GolemInit_BodyPosition(body));
}

static inline void GolemInit_SetRouteReset(struct GolemInitActor *actor, void *callback)
{
    actor->field248 = callback;
}
static inline void GolemInit_SetReactionGate(struct GolemInitActor *actor, void *callback)
{
    actor->field23C = callback;
}

void Golem_InitAnimationProfile(void *);
void Golem_InitColliders(void *);
void Golem_InitActionHandlers(void *);
void Golem_InitSpecialActionHandlers(void *);
s32 Golem_OnUpdate(void *);
u16 Skeleton_LookupAnimationEntry(void *);
void sub_080C5930(void *);
void Golem_UpdateIdleState(void *);
void Golem_UpdateIdle(void *);
void Golem_ResumeBehaviorAfterDisplacement(void *);
void Golem_UpdateDefeat(void *);
void Golem_UpdateRevivalReturn(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Golem_UpdateDying(void);
void Golem_ResumeSavedTarget(void);
void Golem_OnUnusedState(void);
void Golem_ResetCombatState(void);
void SceneActorControl_RefreshMotionRoute(void);
u8 Golem_UpdateHitReaction(void *);
void Golem_ClearControlFlag(void);
void Golem_RestoreRoute(void);
void Golem_OnSharedActorUpdate(void);
void Golem_OnDestroy(void);

static inline void GolemInit_SetContactCallback(struct GolemInitActor *actor, void *callback)
{
    actor->field268 = callback;
}

static inline void GolemInit_EnableInitialFlags(struct GolemInitActor *actor)
{
    GolemInit_SetTargetingFlags(&actor->targeting, 1);
    GolemInit_SetBehaviorFlags(actor, 0x1600000);
    GolemInit_SetStatusFlags(actor, 0x8000008);
}

static inline void GolemInit_ReadInitialMode(struct GolemInitActor *arg0,
                                             struct GolemInitGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.field5CC = 0x21;
    if (i != 0) {
        GolemInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            GolemInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void GolemInit_InitializeRegion(struct GolemInitActor *arg0)
{
    if (GolemInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline struct GolemInitContext *GolemInit_Context(struct GolemInitActor *actor)
{
    return actor->context;
}
static inline void GolemInit_InitializeReceiver(struct GolemInitActor *actor)
{
    actor->field304 = 0;
    GolemInit_Context(actor)->rangeSquared62C = actor->targeting.fieldF4;
}

static inline void GolemInit_InstallCompletionCallbacks(struct GolemInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Golem_OnSharedActorUpdate;
    arg0->field25C = Golem_OnDestroy;
}

static inline void GolemInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void GolemInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                              struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct GolemResourceBlock *)drawable = *(struct GolemResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void GolemInit_InitializePaletteRange(struct GolemInitActor *actor, u32 alternate,
                                                    u32 current)
{
    actor->field1DC = GolemInit_GetPaletteBase(actor) + alternate;
    actor->field1DE = GolemInit_GetPaletteBase(actor) + current;
}

s32 Golem_Init(void *object)
{
    struct GolemInitActor *arg0 = object;
    struct GolemInitActor *actor = object;
    struct GolemInitGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    GolemInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    GolemInit_SetCategory(actor, 3);
    Golem_InitAnimationProfile(actor);
    if (GolemInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = GolemInit_Drawable(actor);
        GolemInit_InitializeSprite(heap, 0x5290, 0x1E38);
        if (GolemInit_GetCategory(actor) == 0x11 || GolemInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = GolemInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        GolemInit_InitializeWidget(heap, 0xCB05, 0x5290, &tmp);
        actor->field2B7 = 0xFF;
    }
    GolemInit_InitializeBody(arg0, &zero);
    GolemInit_ReadPalette(actor, 0x27D, 0x192);
    GolemInit_InitializePaletteRange(actor, 2, 0);
    Golem_InitColliders(actor);
    GolemInit_InstallPrimary(
        actor, Golem_OnUpdate, Skeleton_LookupAnimationEntry, sub_080C5930, Golem_UpdateIdleState,
        Golem_UpdateIdle, Golem_UpdateRevivalReturn, Golem_ResumeBehaviorAfterDisplacement,
        Golem_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_0807F81C,
        SceneActorTarget_ResolvePendingObject, sub_081363F4, SceneActorPlacement_ResetStateThree);
    GolemInit_InitializeGroup(group, (u8 *)arg0 + 0x638, (u8 *)arg0 + 0x65C, Golem_UpdateDying,
                              Golem_ResumeSavedTarget, SceneActorReaction_BeginTargetRecovery,
                              Golem_OnUnusedState, &actor->body);
    SceneActorMovement_ResolveRoutePosition(actor);
    GolemInit_SetRouteReset(actor, Golem_ResetCombatState);
    GolemInit_SetReactionGate(actor, SceneActorControl_RefreshMotionRoute);
    GolemInit_SetContactCallback(actor, Golem_UpdateHitReaction);
    group->field61C = (u32)Golem_ClearControlFlag;
    group->field5EC = Golem_RestoreRoute;
    actor->field2E8 = 0;
    GolemInit_EnableInitialFlags(actor);
    GolemInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    GolemInit_ReadInitialMode(actor, group);
    flag = 0;
    if (GolemInit_TestMotionFlags(actor, 4)) {
        GolemInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        GolemInit_SetState(actor, 0, 0, 0);
        GolemInit_SetControl(actor, GolemInit_GetStandingControl(actor), 0);
    }
    GolemInit_InitializeRegion(actor);
    if (GolemInit_TestMotionFlags(actor, 0x40))
        GolemInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !GolemInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (GolemInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    GolemInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Golem_InitActionHandlers(actor);
    Golem_InitSpecialActionHandlers(actor);

    arg0->field68D = 0;
    arg0->field680 = 0x78;
    arg0->field686 = 0;
    arg0->field684 = 2;
    arg0->field682 = 0;
    arg0->field680 = 0xA0;
    arg0->field68A = 0;
    arg0->field624 = 0;
    arg0->field693 = 0;

    GolemInit_SaveControl(GolemInit_SavedControlOf(actor), actor);
    GolemInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
