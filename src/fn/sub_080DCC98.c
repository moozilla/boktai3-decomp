#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"

s32 Script_GetValue(void);
u32 Script_GetPc(void);
static inline s32 MummyInit_Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u8 MummyInit_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u16 MummyInit_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

struct MummyResourceBlock {
    u32 w[8];
};

s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
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
void *TerrainModifiers_FindCellOverride(s32, s32);
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

struct MummyInitPosition {
    s16 x, y, z, pad;
};
struct MummyInitMap {
    u32 header;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
static inline void MummyInit_ClearPosition(u32 *fill, void *object)
{
    struct MummyInitPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}
static inline u16 MummyInit_PositionX(const struct MummyInitPosition *pos)
{
    return pos->x;
}
static inline u16 MummyInit_PositionZ(const struct MummyInitPosition *pos)
{
    return pos->z;
}
static inline u32 MummyInit_Cell(struct MummyInitPosition *pos)
{
    s32 x = (s16)MummyInit_PositionX(pos) >> 8;
    s32 z = (s16)MummyInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct MummyInitMap *)gTerrainMap)->rows[z] + x;
}
static inline u8 MummyInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}
static inline u32 MummyInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}
static inline u32 MummyInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct MummyInitMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    shape = MummyInit_TerrainShape(p);
    height = MummyInit_TerrainBaseHeight(p);
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

struct MummyInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct MummyInitPosition position;
    u8 unknown64[0x34];
};
struct MummyInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct MummyInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct MummyInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct MummyInitRoutePoint {
    s16 x, z;
};
struct MummyInitRoute {
    u32 unknown0, unknown4;
    struct MummyInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct MummyInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct MummyInitGate gate;
    struct MummyInitRange range;
    u8 unknown42C[0x10];
    void *field43C;
    u32 field440;
    void *field444;
    struct MummyInitRoute route;
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
struct MummyInitTargeting {
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
struct MummyInitSavedControl {
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
struct MummyInitContext {
    u8 unknown0[0x6CC];
    u16 field6CC, field6CE;
    u8 unknown6D0[8];
    u32 field6D8, field6DC;
};
struct MummyInitActor {
    u8 unknown0[0x4];
    u8 *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct MummyInitBody body;
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
    struct MummyInitAttacks attacks;
    struct MummyInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct MummyInitSavedControl saved;
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
    struct MummyInitContext *context;
    struct MummyInitGroup group;
    u8 unknown624[0xC4];
    u16 field6E8;
    u8 unknown6EA[8];
    u16 field6F2;
};

static inline u8 *MummyInit_Drawable(struct MummyInitActor *actor)
{
    return actor->drawable;
}
static inline u8 MummyInit_GetDrawableKind(struct MummyInitActor *actor)
{
    return actor->drawableKind;
}
static inline void MummyInit_SetCategory(void *object, u8 category)
{
    struct MummyInitActor *actor = object;
    actor->category = category;
}
static inline u8 MummyInit_GetCategory(void *object)
{
    struct MummyInitActor *actor = object;
    return actor->category;
}

static inline u16 MummyInit_GetInitialHealth(struct MummyInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 MummyInit_GetPaletteBase(struct MummyInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 MummyInit_GetPaletteOffset(struct MummyInitActor *actor)
{
    return actor->field1E6;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080D7568(void *);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
extern const u8 gUnk_086060FC[];

static inline u32 MummyInit_GetMotionFlags(struct MummyInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 MummyInit_GetHeightOffset(struct MummyInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 MummyInit_GetBehaviorFlags(struct MummyInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 MummyInit_GetStatusFlags(struct MummyInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 MummyInit_GetState(struct MummyInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 MummyInit_GetPhase(struct MummyInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 MummyInit_GetBehavior(struct MummyInitActor *actor)
{
    return actor->field2AA;
}
static inline void *MummyInit_GetCurrentControl(struct MummyInitActor *actor)
{
    return actor->field21C;
}
static inline u8 MummyInit_GetControlKind(struct MummyInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 MummyInit_GetDirection(struct MummyInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 MummyInit_GetTerrainCell(struct MummyInitActor *actor)
{
    return actor->field2F4;
}
static inline u8 MummyInit_GetTargetingFlags(struct MummyInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct MummyInitSavedControl *MummyInit_SavedControlOf(struct MummyInitActor *actor)
{
    struct MummyInitSavedControl *saved = &actor->saved;
    return saved;
}
static inline struct MummyInitPosition *MummyInit_BodyPosition(void *object)
{
    struct MummyInitBody *body = object;
    struct MummyInitPosition *position = &body->position;
    return position;
}
static inline void MummyInit_SaveControl(struct MummyInitSavedControl *saved,
                                         struct MummyInitActor *actor)
{
    saved->field12E = MummyInit_GetMotionFlags(actor);
    saved->field12C = MummyInit_GetHeightOffset(actor);
    saved->field118 = MummyInit_GetBehaviorFlags(actor);
    saved->field11C = MummyInit_GetStatusFlags(actor);
    saved->field120 = MummyInit_GetDirection(actor);
    saved->field121 = MummyInit_GetState(actor);
    saved->field122 = MummyInit_GetPhase(actor);
    saved->field123 = MummyInit_GetBehavior(actor);
    saved->field124 = MummyInit_GetCurrentControl(actor);
    saved->field128 = MummyInit_GetControlKind(actor);
}
struct MummyInitCollider {
    u8 header[6];
    u16 flags;
};
static inline void MummyInit_ClearColliderFlags(struct MummyInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 MummyInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void MummyInit_SetBehaviorFlags(struct MummyInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void MummyInit_SetStatusFlags(struct MummyInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline u8 MummyInit_TestMotionFlags(struct MummyInitActor *actor, u32 mask)
{
    if (MummyInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 MummyInit_TestTargetingFlags(struct MummyInitTargeting *target, u32 mask)
{
    if (MummyInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void MummyInit_SetTargetingFlags(struct MummyInitTargeting *target, u8 mask)
{
    mask |= target->fieldEB;
    target->fieldEB = mask;
}
static inline void MummyInit_ClearControlFlags(struct MummyInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 MummyInit_IsInCurrentRegion(struct MummyInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *MummyInit_GetStandingControl(struct MummyInitActor *actor)
{
    return actor->field22C;
}
static inline void MummyInit_SetControl(struct MummyInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    MummyInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void MummyInit_SetState(struct MummyInitActor *actor, u32 state, u32 phase,
                                      u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}
static inline void MummyInit_ResetFlags(void *object)
{
    struct MummyInitActor *actor = object;
    actor->field6F2 = 0;
}

static inline void MummyInit_InitializeGate(struct MummyInitGate *gate, u16 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;
}

static inline void MummyInit_ReadTargeting(struct MummyInitTargeting *target)
{
    s32 nearRadius, flags, farRadius;
    if (Script_SeekToKeyword(0x65)) {
        nearRadius = MummyInit_Value(0x30);
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
    if (MummyInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);
}

static inline u16 MummyInit_RangeRadius(struct MummyInitRange *range)
{
    return range->field422;
}
static inline void MummyInit_ReadRange(struct MummyInitRange *range)
{
    s32 radius;
    if (Script_SeekToKeyword(0x6E)) {
        radius = MummyInit_Value(0);
        range->field420 = MummyInit_ReadHalfword(0);
    } else {
        radius = 0;
        range->field420 = radius;
    }
    range->field422 = radius;
    range->field424 = MummyInit_RangeRadius(range) * MummyInit_RangeRadius(range);
    range->field42A = 0;
}

static inline u32 MummyInit_RoutePointX(struct MummyInitRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 MummyInit_RoutePointZ(struct MummyInitRoute *route)
{
    return (u16)route->point->z;
}
static inline void MummyInit_SnapToRoute(struct MummyInitPosition *position,
                                         struct MummyInitRoute *route)
{
    position->x = MummyInit_RoutePointX(route);
    position->z = MummyInit_RoutePointZ(route);
    position->y = MummyInit_Height(MummyInit_Cell(position), (u16 *)position);
}
static inline void MummyInit_DrawablePosition(struct MummyInitActor *actor)
{
    if (MummyInit_GetDrawableKind(actor) == 0)
        *(struct MummyInitPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct MummyInitPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void MummyInit_SetFirstAttack(struct MummyInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u16 MummyInit_KeywordHalfword(u32 keyword, s32 fallback)
{
    return Script_GetKeywordValue(keyword, fallback);
}

static inline u8 MummyInit_RouteId(struct MummyInitRoute *route)
{
    return route->routeId;
}
static inline u8 MummyInit_RouteParameter(struct MummyInitRoute *route)
{
    return route->parameterB;
}
static inline u8 MummyInit_RoutePointId(struct MummyInitRoute *route)
{
    return route->pointId;
}
static inline u8 MummyInit_RouteActiveBranch(struct MummyInitRoute *route)
{
    return route->activeBranch;
}
static inline u32 MummyInit_RouteAlternateBranch(struct MummyInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void MummyInit_BeginRoute(struct MummyInitBody *body, struct MummyInitRoute *route)
{
    SceneBody_StartRoute(body, route, MummyInit_RouteId(route), MummyInit_RouteParameter(route),
                         MummyInit_RoutePointId(route));
}
static inline s32 MummyInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void MummyInit_InstallIdle(struct MummyInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void MummyInit_InitializeHealth(struct MummyInitActor *arg0, u16 initialHealth,
                                              s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    MummyInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = MummyInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;
}

static inline void MummyInit_SetInitialHealth(struct MummyInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 MummyInit_ReadRoute(struct MummyInitActor *arg0, struct MummyInitGroup *group,
                                      struct MummyInitRoute *rt)
{
    struct MummyInitPosition point;
    s32 pointId;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = MummyInit_ReadByte(0);
        rt->parameterB = 0;
        pointId = MummyInit_Value(0);
        rt->pointId = pointId;
        rt->currentPoint = pointId;
        rt->branches[MummyInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[MummyInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : MummyInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, MummyInit_RouteId(rt), MummyInit_RoutePointId(rt));
        arg0->field2F4 = MummyInit_Cell(&point);
        point.y = arg0->field2BE = MummyInit_Height(MummyInit_GetTerrainCell(arg0), (u16 *)&point);
        MummyInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[MummyInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[MummyInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void MummyInit_InitializeBody(void *object, u32 *fill)
{
    struct MummyInitActor *arg0 = object;
    struct MummyInitGroup *group = &arg0->group;
    struct MummyInitBody *body = &arg0->body;
    struct MummyInitPosition origin;
    s32 mode, i, secondLimit, firstLimit, gateDuration, identity;
    struct MummyInitRoute *rt;
    arg0->field2CC = 0;
    arg0->field2D0 = 0;
    arg0->field2B4 = 0;
    arg0->field2E8 = 8;
    arg0->field2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    MummyInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        i = 0;
        while (1) {
            MummyInit_SetFirstAttack(arg0, i, MummyInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = MummyInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = MummyInit_Value(0);
            arg0->attacks.parameter[i] = MummyInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MummyInit_Value(0x12C);
        secondLimit = MummyInit_Value(0x12C);
        gateDuration = MummyInit_Value(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    MummyInit_InitializeGate(&group->gate, gateDuration);
    MummyInit_ReadRange(&group->range);
    MummyInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        MummyInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MummyInit_KeywordHalfword(0x46, 0);
    arg0->field1EC = MummyInit_KeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MummyInit_SetInitialHealth(arg0, MummyInit_ReadHalfword(0));
        group->field5B6 = MummyInit_Half(MummyInit_GetInitialHealth(arg0));
        arg0->field9E = MummyInit_ReadByte(0);
        arg0->field9F = MummyInit_ReadByte(0);
        arg0->fieldA0 = MummyInit_ReadByte(0);
        arg0->fieldA2 = MummyInit_ReadHalfword(0);
        arg0->fieldA4 = MummyInit_ReadHalfword(0);
    }

    MummyInit_InitializeHealth(arg0, MummyInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MummyInit_Value(0);
        arg0->fieldA1 = MummyInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->field9A = identity;
    if (Script_GetKeywordValue(0x48, 0))
        MummyInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MummyInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (MummyInit_ReadRoute(arg0, group, rt)) {
        MummyInit_SnapToRoute(&arg0->body.position, rt);
        MummyInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MummyInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, MummyInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MummyInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct MummyInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MummyInit_SetWidgetScriptPalette(struct MummyInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void MummyInit_SetMotionControl(struct MummyInitActor *actor, void *callback,
                                              u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void MummyInit_ClearMotion(void *object)
{
    struct MummyInitActor *actor = object;
    MummyInit_SetMotionControl(actor, 0, 0);
    actor->field114 = 0;
}
static inline void MummyInit_ReadPalette(struct MummyInitActor *actor, u32 base, u32 alternate)
{
    s32 palette;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = MummyInit_GetPaletteOffset(actor) + base;
    palette = MummyInit_GetPaletteBase(actor) + MummyInit_GetPaletteOffset(actor);
    if (MummyInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MummyInit_GetDrawableKind(actor) == 1)
        MummyInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                         SceneActorScript_GetManagerField62());
}

static inline void MummyInit_InstallPrimary(struct MummyInitActor *arg0, s32 (*cb0)(void *),
                                            u16 (*cb1)(void *), void (*cb2)(void *),
                                            void (*cb3)(void *), void (*cb4)(void *),
                                            void (*hidden)(void *), void (*cb5)(void *),
                                            void (*cb6)(void *), u32 (*defaultReaction)(void),
                                            u8 (*cb270)(void *), void *targetQuery, void *damage,
                                            void *contact)
{
    MummyInit_SetBehaviorFlags(arg0, 0x2000);
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
    MummyInit_ClearMotion(arg0);
}

static inline void MummyInit_InitializeGroup(struct MummyInitGroup *group, void *firstBounds,
                                             void *secondBounds, void *downed, void *returnToRoute,
                                             void *recover, void *animate, u8 active,
                                             struct MummyInitBody *body)
{
    group->field5D0 = firstBounds;
    group->field5D4 = secondBounds;
    group->field5F4 = downed;
    group->field5F8 = recover;
    group->field5FC = SceneActorReaction_BeginFadedState;
    group->field600 = SceneActorReaction_BeginDefeatBurst;
    group->field43C = returnToRoute;
    group->field440 = 0;
    group->range.field42B = active;
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
    ProximityVolume_TranslateBounds(firstBounds, MummyInit_BodyPosition(body));
}

static inline void MummyInit_SetRouteReset(struct MummyInitActor *actor, void *callback)
{
    actor->field248 = callback;
}
static inline void MummyInit_SetReactionGate(struct MummyInitActor *actor, void *callback)
{
    actor->field23C = callback;
}

void Mummy_InitProfile(void *);
void Mummy_InitCollision(void *);
void Mummy_InitActionHandlers(void *);
void Mummy_SetBehaviorTable(void *);
s32 Mummy_OnUpdate(void *);
u16 Mummy_GetAnimationStepOffset(void *);
void Mummy_TurnTowardRoute(void *);
void Mummy_UpdateWalking(void *);
void Mummy_UpdateIdle(void *);
void Mummy_UpdateDisplacementRecovery(void *);
void Mummy_UpdateDefeat(void *);
void Mummy_UpdateHiddenRecovery(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Mummy_UpdateDying(void);
void Mummy_ResumeSavedTarget(void);
void Mummy_OnUnusedState(void);
void Mummy_ResetRevivalMotion(void);
void SceneActor_EnterMotionPhase(void);
u8 Mummy_SelectMovementResponse(void *);
void Mummy_OnClearActivityFlag(void);
void Mummy_OnMovementUpdate(void);
void Mummy_OnInteractionUpdate(void);
void Mummy_OnReleaseResources(void);

static inline void MummyInit_SetContactCallback(struct MummyInitActor *actor, void *callback)
{
    actor->field268 = callback;
}

static inline void MummyInit_EnableInitialFlags(struct MummyInitActor *actor)
{
    MummyInit_SetTargetingFlags(&actor->targeting, 1);
    MummyInit_SetBehaviorFlags(actor, 0x600000);
    MummyInit_SetStatusFlags(actor, 0x8000000);
}

static inline void MummyInit_ReadInitialMode(struct MummyInitActor *arg0,
                                             struct MummyInitGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.field5CC = 0x21;
    if (i != 0) {
        MummyInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            MummyInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void MummyInit_InitializeRegion(struct MummyInitActor *arg0)
{
    if (MummyInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline struct MummyInitContext *MummyInit_Context(struct MummyInitActor *actor)
{
    return actor->context;
}
static inline void MummyInit_InitializeReceiver(struct MummyInitActor *actor)
{
    actor->field304 = 0;
}

static inline void MummyInit_InstallCompletionCallbacks(struct MummyInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Mummy_OnInteractionUpdate;
    arg0->field25C = Mummy_OnReleaseResources;
}

static inline void MummyInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void MummyInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                              struct MummyInitPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct MummyResourceBlock *)drawable = *(struct MummyResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void MummyInit_InitializePaletteRange(struct MummyInitActor *actor, u32 selected,
                                                    u32 alternate, u32 current)
{
    actor->field1E4 = selected;
    actor->field1DC = MummyInit_GetPaletteBase(actor) + alternate;
    actor->field1DE = MummyInit_GetPaletteBase(actor) + current;
}

void Mummy_SetControlPreset(void *);
void SceneActorEffects_InitAtActorPosition(void *);
void sub_080D1644(void);
void sub_080D453C(void);

s32 Mummy_Init(void *object)
{
    struct MummyInitActor *arg0 = object;
    struct MummyInitActor *actor = object;
    struct MummyInitGroup *group = &arg0->group;
    u32 zero;
    struct MummyInitPosition tmp;
    u32 flag;

    MummyInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MummyInit_SetCategory(actor, 1);
    Mummy_InitProfile(actor);
    if (MummyInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MummyInit_Drawable(actor);
        MummyInit_InitializeSprite(heap, 0x48DE, 0xA4B2);
        if (MummyInit_GetCategory(actor) == 0x11 || MummyInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MummyInit_Drawable(actor);
        MummyInit_ClearPosition(&zero, &tmp);
        MummyInit_InitializeWidget(heap, 0xCB05, 0x48DE, &tmp);
        actor->field2B7 = 0xFF;
    }
    MummyInit_InitializeBody(arg0, &zero);
    MummyInit_ReadPalette(actor, 0x23, 0x192);
    MummyInit_InitializePaletteRange(actor, MummyInit_GetPaletteBase(actor) + 3, 4, 0);
    Mummy_InitCollision(actor);
    MummyInit_InstallPrimary(actor, Mummy_OnUpdate, Mummy_GetAnimationStepOffset,
                             Mummy_TurnTowardRoute, Mummy_UpdateWalking, Mummy_UpdateIdle,
                             Mummy_UpdateHiddenRecovery, Mummy_UpdateDisplacementRecovery,
                             Mummy_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_080D7568,
                             SceneActorTarget_ResolvePendingObject, sub_081363F4,
                             SceneActorPlacement_ResetStateThree);
    MummyInit_InitializeGroup(group, (u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x718, Mummy_UpdateDying,
                              Mummy_ResumeSavedTarget, SceneActorReaction_BeginTargetRecovery,
                              Mummy_OnUnusedState, 3, &actor->body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    MummyInit_SetRouteReset(actor, Mummy_ResetRevivalMotion);
    MummyInit_SetReactionGate(actor, SceneActor_EnterMotionPhase);
    MummyInit_SetContactCallback(actor, Mummy_SelectMovementResponse);
    group->field61C = (u32)Mummy_OnClearActivityFlag;
    group->field5EC = Mummy_OnMovementUpdate;
    group->field5E8 = (u32)sub_080D1644;
    group->field444 = sub_080D453C;
    actor->field2E8 = 1;
    MummyInit_EnableInitialFlags(actor);
    MummyInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorEffects_InitAtActorPosition(arg0);
    SceneActorTerrain_ReadScriptParameters(actor);
    MummyInit_ReadInitialMode(arg0, group);
    flag = 0;
    if (MummyInit_TestMotionFlags(actor, 4)) {
        MummyInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MummyInit_SetState(actor, 0, 0, 0);
        MummyInit_SetControl(actor, MummyInit_GetStandingControl(actor), 0);
    }
    MummyInit_InitializeRegion(actor);
    if (MummyInit_TestMotionFlags(actor, 0x40))
        MummyInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MummyInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MummyInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MummyInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Mummy_SetControlPreset(actor);
    Mummy_InitActionHandlers(actor);
    Mummy_SetBehaviorTable(actor);

    arg0->field6E8 = actor->targeting.fieldEC;
    {
        struct MummyInitContext *context = MummyInit_Context(actor);
        if (Script_SeekToKeyword(0x42)) {
            context->field6CC = MummyInit_ReadHalfword(0x14);
            context->field6CE = MummyInit_ReadHalfword(0xB4);
        } else {
            context->field6CC = 0x14;
            context->field6CE = 0xB4;
        }
    }
    {
        struct MummyInitContext *context = MummyInit_Context(actor);
        context->field6D8 = Div(actor->targeting.fieldF4 * 3, 10);
        context->field6DC = actor->targeting.fieldF4;
    }

    MummyInit_SaveControl(MummyInit_SavedControlOf(actor), actor);
    MummyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
