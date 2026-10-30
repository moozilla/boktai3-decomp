#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"

s32 Script_GetValue(void);
u32 Script_GetPc(void);
static inline s32 VornInit_Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u8 VornInit_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u16 VornInit_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

struct VornResourceBlock {
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

struct VornInitPosition {
    s16 x, y, z, pad;
};
struct VornInitMap {
    u32 header;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
static inline void VornInit_ClearPosition(u32 *fill, void *object)
{
    struct VornInitPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}
static inline u16 VornInit_PositionX(const struct VornInitPosition *pos)
{
    return pos->x;
}
static inline u16 VornInit_PositionZ(const struct VornInitPosition *pos)
{
    return pos->z;
}
static inline u32 VornInit_Cell(struct VornInitPosition *pos)
{
    s32 x = (s16)VornInit_PositionX(pos) >> 8;
    s32 z = (s16)VornInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct VornInitMap *)gTerrainMap)->rows[z] + x;
}
static inline u8 VornInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}
static inline u32 VornInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}
static inline u32 VornInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct VornInitMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    shape = VornInit_TerrainShape(p);
    height = VornInit_TerrainBaseHeight(p);
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

struct VornInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct VornInitPosition position;
    u8 unknown64[0x34];
};
struct VornInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct VornInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct VornInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct VornInitRoutePoint {
    s16 x, z;
};
struct VornInitRoute {
    u32 unknown0, unknown4;
    struct VornInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct VornInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct VornInitGate gate;
    struct VornInitRange range;
    u8 unknown42C[0x10];
    void *field43C;
    u32 field440;
    void *field444;
    struct VornInitRoute route;
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
struct VornInitTargeting {
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
struct VornInitSavedControl {
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
struct VornInitActor {
    u8 unknown0[0x4];
    u8 *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct VornInitBody body;
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
    struct VornInitAttacks attacks;
    struct VornInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct VornInitSavedControl saved;
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
    void *context;
    struct VornInitGroup group;
    u8 unknown624[0x68];
    u8 field68C;
    u8 unknown68D[0x2C];
    u8 field6B9;
    u8 unknown6BA[0x1A];
    u16 field6D4;
};

static inline u8 *VornInit_Drawable(struct VornInitActor *actor)
{
    return actor->drawable;
}
static inline u8 VornInit_GetDrawableKind(struct VornInitActor *actor)
{
    return actor->drawableKind;
}
static inline void VornInit_SetCategory(void *object, u8 category)
{
    struct VornInitActor *actor = object;
    actor->category = category;
}
static inline u8 VornInit_GetCategory(void *object)
{
    struct VornInitActor *actor = object;
    return actor->category;
}

static inline u16 VornInit_GetInitialHealth(struct VornInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 VornInit_GetPaletteBase(struct VornInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 VornInit_GetPaletteOffset(struct VornInitActor *actor)
{
    return actor->field1E6;
}

void SceneActorBehavior_InitializeShadow(void *);

void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_080B3C1C(void *);
void SceneActorPlacement_ResetStateThree(void);
extern const u8 gUnk_08606038[];

static inline u32 VornInit_GetMotionFlags(struct VornInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 VornInit_GetHeightOffset(struct VornInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 VornInit_GetBehaviorFlags(struct VornInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 VornInit_GetStatusFlags(struct VornInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 VornInit_GetState(struct VornInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 VornInit_GetPhase(struct VornInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 VornInit_GetBehavior(struct VornInitActor *actor)
{
    return actor->field2AA;
}
static inline void * VornInit_GetCurrentControl(struct VornInitActor *actor)
{
    return actor->field21C;
}
static inline u8 VornInit_GetControlKind(struct VornInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 VornInit_GetDirection(struct VornInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 VornInit_GetTerrainCell(struct VornInitActor *actor)
{
    return actor->field2F4;
}
static inline u32 VornInit_GetTargetingFlags(struct VornInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct VornInitSavedControl *VornInit_SavedControlOf(struct VornInitActor *actor)
{
    struct VornInitSavedControl *saved = &actor->saved;
    return saved;
}
static inline struct VornInitPosition *VornInit_BodyPosition(void *object)
{
    struct VornInitBody *body = object;
    struct VornInitPosition *position = &body->position;
    return position;
}
static inline void VornInit_SaveControl(struct VornInitSavedControl *saved, struct VornInitActor *actor)
{
    saved->field12E = VornInit_GetMotionFlags(actor);
    saved->field12C = VornInit_GetHeightOffset(actor);
    saved->field118 = VornInit_GetBehaviorFlags(actor);
    saved->field11C = VornInit_GetStatusFlags(actor);
    saved->field120 = VornInit_GetDirection(actor);
    saved->field121 = VornInit_GetState(actor);
    saved->field122 = VornInit_GetPhase(actor);
    saved->field123 = VornInit_GetBehavior(actor);
    saved->field124 = VornInit_GetCurrentControl(actor);
    saved->field128 = VornInit_GetControlKind(actor);
}
struct VornInitCollider { u8 header[6]; u16 flags; };
static inline void VornInit_ClearColliderFlags(struct VornInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 VornInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void VornInit_SetBehaviorFlags(struct VornInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void VornInit_SetStatusFlags(struct VornInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline u8 VornInit_TestMotionFlags(struct VornInitActor *actor, u32 mask)
{
    if (VornInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 VornInit_TestTargetingFlags(struct VornInitTargeting *target, u32 mask)
{
    if (VornInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void VornInit_SetTargetingFlags(struct VornInitTargeting *target, u8 mask)
{
    mask |= target->fieldEB;
    target->fieldEB = mask;
}
static inline void VornInit_ClearControlFlags(struct VornInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 VornInit_IsInCurrentRegion(struct VornInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *VornInit_GetStandingControl(struct VornInitActor *actor)
{
    return actor->field22C;
}
static inline void VornInit_SetControl(struct VornInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    VornInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void VornInit_SetState(struct VornInitActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}
static inline void VornInit_BeginBehavior(struct VornInitActor *actor, u32 behavior)
{
    VornInit_SetState(actor, 0, 0, behavior);
}
static inline void VornInit_ResetFlags(void *object)
{
    struct VornInitActor *actor = object;
    actor->field6D4 = 0;
}

static inline void VornInit_InitializeGate(struct VornInitGate *gate, u16 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;

}

static inline void VornInit_ReadTargeting(struct VornInitTargeting *target)
{
    s32 nearRadius, flags, farRadius;
    if (Script_SeekToKeyword(0x65)) {
        nearRadius = VornInit_Value(0x30);
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
    target->fieldF8 = nearRadius *nearRadius;
    target->fieldFC = farRadius *farRadius;
    target->fieldEA = flags;
    if (VornInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);

}

static inline u16 VornInit_RangeRadius(struct VornInitRange *range)
{
    return range->field422;
}
static inline void VornInit_ReadRange(struct VornInitRange *range)
{
    s32 radius;
    if (Script_SeekToKeyword(0x6E)) {
        radius = VornInit_Value(0);
        range->field420 = VornInit_ReadHalfword(0);
    } else {
        radius = 0;
        range->field420 = radius;
    }
    range->field422 = radius;
    range->field424 = VornInit_RangeRadius(range) * VornInit_RangeRadius(range);
    range->field42A = 0;

}

static inline u32 VornInit_RoutePointX(struct VornInitRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 VornInit_RoutePointZ(struct VornInitRoute *route)
{
    return (u16)route->point->z;
}
static inline void VornInit_SnapToRoute(struct VornInitPosition *position, struct VornInitRoute *route)
{
    position->x = VornInit_RoutePointX(route);
    position->z = VornInit_RoutePointZ(route);
    position->y = VornInit_Height(VornInit_Cell(position), (u16 *)position);
}
static inline void VornInit_DrawablePosition(struct VornInitActor *actor)
{
    if (VornInit_GetDrawableKind(actor) == 0)
        *(struct VornInitPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct VornInitPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void VornInit_SetFirstAttack(struct VornInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u16 VornInit_KeywordHalfword(u32 keyword, s32 fallback)
{
    return Script_GetKeywordValue(keyword, fallback);
}

static inline u8 VornInit_RouteId(struct VornInitRoute *route)
{
    return route->routeId;
}
static inline u8 VornInit_RouteParameter(struct VornInitRoute *route)
{
    return route->parameterB;
}
static inline u8 VornInit_RoutePointId(struct VornInitRoute *route)
{
    return route->pointId;
}
static inline u8 VornInit_RouteActiveBranch(struct VornInitRoute *route)
{
    return route->activeBranch;
}
static inline u32 VornInit_RouteAlternateBranch(struct VornInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void VornInit_BeginRoute(struct VornInitBody *body, struct VornInitRoute *route)
{
    SceneBody_StartRoute(body, route, VornInit_RouteId(route), VornInit_RouteParameter(route), VornInit_RoutePointId(route));
}
static inline s32 VornInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void VornInit_InstallIdle(struct VornInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void VornInit_InitializeHealth(struct VornInitActor *arg0,
    u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    VornInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = VornInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;

}

static inline void VornInit_SetInitialHealth(struct VornInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 VornInit_ReadRoute(struct VornInitActor *arg0, struct VornInitGroup *group, struct VornInitRoute *rt)
{
    struct VornInitPosition point;
    s32 pointId;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = VornInit_ReadByte(0);
        rt->parameterB = 0;
        pointId = VornInit_Value(0);
        rt->pointId = pointId;
        rt->currentPoint = pointId;
        rt->branches[VornInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[VornInit_RouteAlternateBranch(rt)] = Script_GetPc() ? Script_GetValue() : VornInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, VornInit_RouteId(rt), VornInit_RoutePointId(rt));
        arg0->field2F4 = VornInit_Cell(&point);
        point.y = arg0->field2BE = VornInit_Height(VornInit_GetTerrainCell(arg0), (u16 *)&point);
        VornInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[VornInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[VornInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void VornInit_InitializeBody(void *object, u32 *fill)
{
    struct VornInitActor *arg0 = object;
    struct VornInitGroup *group = &arg0->group;
    struct VornInitBody *body = &arg0->body;
    struct VornInitPosition origin;
    s32 mode, i, secondLimit, firstLimit, gateDuration, identity;
    struct VornInitRoute *rt;
    arg0->field2CC = 0;
    arg0->field2D0 = 0;
    arg0->field2B4 = 0;
    arg0->field2E8 = 8;
    arg0->field2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    VornInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        i = 0;
        while (1) {
            VornInit_SetFirstAttack(arg0, i, VornInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = VornInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = VornInit_Value(0);
            arg0->attacks.parameter[i] = VornInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = VornInit_Value(0x12C);
        secondLimit = VornInit_Value(0x12C);
        gateDuration = VornInit_Value(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    VornInit_InitializeGate(&group->gate, gateDuration);
    VornInit_ReadRange(&group->range);
    VornInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        VornInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = VornInit_KeywordHalfword(0x46, 0);
    arg0->field1EC = VornInit_KeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        VornInit_SetInitialHealth(arg0, VornInit_ReadHalfword(0));
        group->field5B6 = VornInit_Half(VornInit_GetInitialHealth(arg0));
        arg0->field9E = VornInit_ReadByte(0);
        arg0->field9F = VornInit_ReadByte(0);
        arg0->fieldA0 = VornInit_ReadByte(0);
        arg0->fieldA2 = VornInit_ReadHalfword(0);
        arg0->fieldA4 = VornInit_ReadHalfword(0);
    }

    VornInit_InitializeHealth(arg0, VornInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = VornInit_Value(0);
        arg0->fieldA1 = VornInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->field9A = identity;
    if (Script_GetKeywordValue(0x48, 0))
        VornInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        VornInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (VornInit_ReadRoute(arg0, group, rt)) {
        VornInit_SnapToRoute(&arg0->body.position, rt);
        VornInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (VornInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, VornInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (VornInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct VornInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void VornInit_SetWidgetScriptPalette(struct VornInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void VornInit_SetMotionControl(struct VornInitActor *actor, void *callback, u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void VornInit_ClearMotion(void *object)
{
    struct VornInitActor *actor = object;
    VornInit_SetMotionControl(actor, 0, 0);
    actor->field114 = 0;
}
static inline void VornInit_ReadPalette(struct VornInitActor *actor, u32 base, u32 alternate)
{
    s32 palette;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = VornInit_GetPaletteOffset(actor) + base + 1;
    palette = VornInit_GetPaletteBase(actor) + VornInit_GetPaletteOffset(actor);
    if (VornInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (VornInit_GetDrawableKind(actor) == 1)
        VornInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20), SceneActorScript_GetManagerField62());
}

static inline void VornInit_InstallPrimary(struct VornInitActor *arg0,
    s32 (*cb0)(void *),
    u16 (*cb1)(void *),
    void (*cb2)(void *),
    void (*cb3)(void *),
    void (*cb4)(void *), void (*hidden)(void *),
    void (*cb5)(void *),
    void (*cb6)(void *), u32 (*defaultReaction)(void), u8 (*cb270)(void *), void *targetQuery, void *damage, void *contact)
{
    VornInit_SetBehaviorFlags(arg0, 0x2000);
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
    VornInit_ClearMotion(arg0);
}

static inline void VornInit_InitializeGroup(struct VornInitGroup *group, void *firstBounds,
    void *secondBounds, void *downed, void *returnToRoute, void *recover,
    void *animate, u8 active, struct VornInitBody *body)
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
    ProximityVolume_TranslateBounds(firstBounds, VornInit_BodyPosition(body));
}

static inline void VornInit_SetRouteReset(struct VornInitActor *actor, void *callback)
{
    actor->field248 = callback;
}
static inline void VornInit_SetReactionGate(struct VornInitActor *actor, void *callback)
{
    actor->field23C = callback;
}

void Vorn_InitActorProfile(void *);
void SceneActor_InitContextColliders(void *);
s32 Vorn_OnUpdate(void *);
u16 Vorn_GetAnimationEvent(void *);
void Vorn_UpdateIdleFacing(void *);
void Vorn_ResetStateEntry(void *);
void Vorn_BeginIdle(void *);
void sub_080AD75C(void *);
void Vorn_UpdateDefeat(void *);
void Vorn_UpdateHiddenRecovery(void *);
void SceneActorTarget_ResolvePendingObject(void);
void Vorn_OnReactionNoOp(void);
void SceneActorReaction_ResumeSavedTarget(void);
void SceneActor_ResetContextAnimation(void);
void sub_080B197C(void);
u8 Vorn_SelectHitRecovery(void *);
void Vorn_OnAnimationCleanup(void);
void Vorn_BeginTargetPursuit(void);
void Vorn_OnRender(void);
void Vorn_OnDestroy(void);

static inline void VornInit_SetReactionTable(struct VornInitActor *actor, const void *table)
{
    actor->field250 = table;
}
static inline void VornInit_SetContactCallback(struct VornInitActor *actor, void *callback)
{
    actor->field268 = callback;
}

static inline void VornInit_EnableInitialFlags(struct VornInitActor *actor)
{
    VornInit_SetTargetingFlags(&actor->targeting, 1);
    VornInit_SetBehaviorFlags(actor, 0x1600000);
    VornInit_SetStatusFlags(actor, 0x8000000);
}

static inline void VornInit_ReadInitialMode(struct VornInitActor *arg0, struct VornInitGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.field5CC = 0x21;
    if (i != 0) {
        VornInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            VornInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void VornInit_InitializeRegion(struct VornInitActor *arg0)
{
    if (VornInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);

}

static inline void VornInit_InitializeReceiver(struct VornInitActor *actor)
{
    actor->field304 = 0;
}

static inline void VornInit_InstallCompletionCallbacks(struct VornInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Vorn_OnRender;
    arg0->field25C = Vorn_OnDestroy;

}

static inline void VornInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void VornInit_InitializeWidget(u8 *drawable, u16 script, u16 resource, struct VornInitPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct VornResourceBlock *)drawable = *(struct VornResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

void Vorn_ParseField624(void *);
void Vorn_InitScriptCallback(void *);
void Vorn_InitActionCooldown(void *);
void Vorn_InitTimedAction(void *);
void Vorn_ParseField69A(void *);
void Vorn_ResetRecoveryMode(void *);
void Vorn_InitMotionLimits(void *);
void Vorn_ParseField688(void *);
void Vorn_InitAttackActionTable(void *);
void Vorn_InitSpecialActionTable(void *);
void Vorn_InitReactionActionTable(void *);
void Vorn_InitActionTable(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void ActorShadow_Disable(void *);
void SceneActorReaction_BeginSideStep(void);
s32 Vorn_TryRestorePatrolRoute(void *);
void Vorn_UpdateKnockbackRecovery(void);
void SceneActorReaction_ResumeOrRunScript(void);
void Vorn_ResumeReactionRoute(void);

s32 Vorn_Init(void *object)
{
    struct VornInitActor *arg0 = object;
    struct VornInitActor *actor = object;
    struct VornInitGroup *group = &arg0->group;
    struct VornInitBody *body = &arg0->body;
    u32 zero;
    struct VornInitPosition tmp;
    u32 flag;
    s32 mode;

    VornInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    VornInit_SetCategory(actor, 0x19);
    Vorn_InitActorProfile(actor);
    Vorn_ParseField624(actor);
    Vorn_InitScriptCallback(actor);
    Vorn_InitActionCooldown(actor);
    Vorn_InitTimedAction(actor);
    Vorn_ParseField69A(actor);
    Vorn_ResetRecoveryMode(actor);
    Vorn_InitMotionLimits(actor);
    Vorn_ParseField688(actor);
    if (VornInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = VornInit_Drawable(actor);
        VornInit_InitializeSprite(heap, 0x3DB6, 0);
        if (VornInit_GetCategory(actor) == 0x11 || VornInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = VornInit_Drawable(actor);
        VornInit_ClearPosition(&zero, &tmp);
        VornInit_InitializeWidget(heap, 0xCB05, 0x3DB6, &tmp);
        actor->field2B7 = 0xFF;
    }
    VornInit_InitializeBody(arg0, &zero);
    VornInit_ReadPalette(actor, 0x31B, 0x192);
    SceneActor_InitContextColliders(actor);
    VornInit_InstallPrimary(actor, Vorn_OnUpdate, Vorn_GetAnimationEvent,
        Vorn_UpdateIdleFacing, Vorn_ResetStateEntry, Vorn_BeginIdle, Vorn_UpdateHiddenRecovery,
        sub_080AD75C, Vorn_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, sub_080B3C1C, SceneActorTarget_ResolvePendingObject,
        sub_081363F4, SceneActorPlacement_ResetStateThree);
    VornInit_InitializeGroup(group, (u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x718,
        Vorn_OnReactionNoOp, SceneActorReaction_ResumeSavedTarget, Vorn_TryRestorePatrolRoute,
        0, 1, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    VornInit_SetRouteReset(actor, SceneActor_ResetContextAnimation);
    VornInit_SetReactionGate(actor, sub_080B197C);
    VornInit_SetContactCallback(actor, Vorn_SelectHitRecovery);
    VornInit_SetReactionTable(actor, gUnk_08606038);
    group->field5DC = Vorn_UpdateKnockbackRecovery;
    group->field5E0 = SceneActorReaction_ResumeOrRunScript;
    group->field61C = (u32)Vorn_OnAnimationCleanup;
    group->field5EC = Vorn_BeginTargetPursuit;
    group->field614 = Vorn_ResumeReactionRoute;
    actor->field2E8 = 3;
    VornInit_EnableInitialFlags(actor);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    VornInit_ReadInitialMode(actor, group);
    flag = 0;
    if (VornInit_TestMotionFlags(actor, 4)) {
        VornInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        VornInit_SetState(actor, 0, 0, 0);
        VornInit_SetControl(actor, VornInit_GetStandingControl(actor), 0);
    }
    VornInit_InitializeRegion(actor);
    if (VornInit_TestMotionFlags(actor, 0x40))
        VornInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !VornInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (VornInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    VornInit_InitializeReceiver(actor);
    SceneActorRegistry_RegisterCategoryReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Vorn_InitAttackActionTable(actor);
    Vorn_InitSpecialActionTable(actor);
    Vorn_InitReactionActionTable(actor);
    Vorn_InitActionTable(actor);
    VornInit_SaveControl(VornInit_SavedControlOf(actor), actor);

    flag = arg0->field68C;
    if (flag == 1) {
        if (VornInit_GetDrawableKind(actor) == 0)
            *(u32 *)actor->drawable |= flag;
        else {
            u8 *widget = actor->drawable + 0x20;
            *(u32 *)(widget + 8) |= flag;
        }
        ActorShadow_Disable((u8 *)actor + 0x320);
        VornInit_SetState(actor, 3, 0, 0);
        VornInit_SetControl(actor, actor->field220, 0xC);
    } else if ((mode = Script_GetKeywordValue(0x54, 0)) != 0) {
        if (mode == 1)
            arg0->field6B9 = 0;
        else
            arg0->field6B9 = 1;
        VornInit_BeginBehavior(actor, 0x12);
        VornInit_SetControl(actor, actor->field22C, 0);
        VornInit_SetMotionControl(actor, SceneActorReaction_BeginSideStep, 0x26);
    }
    VornInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
