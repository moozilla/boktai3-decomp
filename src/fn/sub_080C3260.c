#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"

s32 Script_GetValue(void);
u32 Script_GetPc(void);
static inline s32 SkeletonInit_Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u8 SkeletonInit_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u16 SkeletonInit_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

struct SkeletonResourceBlock {
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

struct SkelPosition {
    s16 x, y, z, pad;
};
struct SkelMap {
    u32 header;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
static inline void SkeletonInit_ClearPosition(u32 *fill, void *object)
{
    struct SkelPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}
static inline u16 SkeletonInit_PositionX(const struct SkelPosition *pos)
{
    return pos->x;
}
static inline u16 SkeletonInit_PositionZ(const struct SkelPosition *pos)
{
    return pos->z;
}
static inline u32 SkeletonInit_Cell(struct SkelPosition *pos)
{
    s32 x = (s16)SkeletonInit_PositionX(pos) >> 8;
    s32 z = (s16)SkeletonInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct SkelMap *)gTerrainMap)->rows[z] + x;
}
static inline u8 SkeletonInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}
static inline u32 SkeletonInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}
static inline u32 SkeletonInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct SkelMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    shape = SkeletonInit_TerrainShape(p);
    height = SkeletonInit_TerrainBaseHeight(p);
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

struct SkeletonBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct SkelPosition position;
    u8 unknown64[0x34];
};
struct SkeletonAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct SkelGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct SkelRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct SkelRoutePoint {
    s16 x, z;
};
struct SkelRoute {
    u32 unknown0, unknown4;
    struct SkelRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct SkeletonGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct SkelGate gate;
    struct SkelRange range;
    u8 unknown42C[0x10];
    void *field43C;
    u32 field440;
    void *field444;
    struct SkelRoute route;
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
struct SkelTargeting {
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
struct SkelSavedControl {
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
struct SkeletonActor {
    u8 unknown0[0x4];
    u8 *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct SkeletonBody body;
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
    struct SkeletonAttacks attacks;
    struct SkelTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct SkelSavedControl saved;
    u8 unknown134[0x6];
    u16 field13A;
    u8 unknown13C[0xA4];
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
    u8 unknown308[0xCC];
    struct SkeletonGroup group;
    u8 unknown624[0xB8];
    u32 field6DC;
    u8 unknown6E0[0x38];
    u32 field718;
};

static inline u8 *SkeletonInit_Drawable(struct SkeletonActor *actor)
{
    return actor->drawable;
}
static inline u8 SkeletonInit_GetDrawableKind(struct SkeletonActor *actor)
{
    return actor->drawableKind;
}
static inline void SkeletonInit_SetCategory(struct SkeletonActor *actor, u8 category)
{
    actor->category = category;
}
static inline u8 SkeletonInit_GetCategory(void *object)
{
    struct SkeletonActor *actor = object;
    return actor->category;
}
static inline s32 SkeletonInit_GetHealth(struct SkeletonActor *actor)
{
    return actor->field9C;
}
static inline u16 SkeletonInit_GetInitialHealth(struct SkeletonActor *actor)
{
    return actor->fieldB0;
}

static inline u32 SkeletonInit_GetPaletteBase(struct SkeletonActor *actor)
{
    return actor->field1E0;
}

static inline u32 SkeletonInit_GetPaletteOffset(struct SkeletonActor *actor)
{
    return actor->field1E6;
}

void SceneActorBehavior_InitializeShadow(void *);
extern u8 *gUnknown_02000580;

void SceneActor_UpdateProfileForCategory(void *);
void SceneActorReaction_ParseOptions(void *);
void SceneActorScript_ReadPParameters(void *);
void SceneActorScript_ReadCallScriptConfig(void *);
void SceneActorControl_InitBehaviorColliders(void *);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
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

static inline u32 SkeletonInit_GetMotionFlags(struct SkeletonActor *actor)
{
    return actor->field2C0;
}
static inline u16 SkeletonInit_GetHeightOffset(struct SkeletonActor *actor)
{
    return actor->field2EA;
}
static inline u32 SkeletonInit_GetBehaviorFlags(struct SkeletonActor *actor)
{
    return actor->field2CC;
}
static inline u32 SkeletonInit_GetStatusFlags(struct SkeletonActor *actor)
{
    return actor->field2D0;
}
static inline u8 SkeletonInit_GetState(struct SkeletonActor *actor)
{
    return actor->field2A8;
}
static inline u8 SkeletonInit_GetPhase(struct SkeletonActor *actor)
{
    return actor->field2A9;
}
static inline u8 SkeletonInit_GetBehavior(struct SkeletonActor *actor)
{
    return actor->field2AA;
}
static inline void *SkeletonInit_GetCurrentControl(struct SkeletonActor *actor)
{
    return actor->field21C;
}
static inline u8 SkeletonInit_GetControlKind(struct SkeletonActor *actor)
{
    return actor->field2B5;
}
static inline u8 SkeletonInit_GetDirection(struct SkeletonActor *actor)
{
    return actor->body.direction;
}
static inline u32 SkeletonInit_GetTerrainCell(struct SkeletonActor *actor)
{
    return actor->field2F4;
}
static inline u8 SkeletonInit_GetTargetingFlags(struct SkelTargeting *target)
{
    return target->fieldEB;
}
static inline struct SkelSavedControl *SkeletonInit_SavedControlOf(struct SkeletonActor *actor)
{
    struct SkelSavedControl *saved = &actor->saved;
    return saved;
}
static inline struct SkelPosition *SkeletonInit_BodyPosition(void *object)
{
    struct SkeletonBody *body = object;
    struct SkelPosition *position = &body->position;
    return position;
}
static inline void SkeletonInit_SaveControl(struct SkelSavedControl *saved,
                                            struct SkeletonActor *actor)
{
    saved->field12E = SkeletonInit_GetMotionFlags(actor);
    saved->field12C = SkeletonInit_GetHeightOffset(actor);
    saved->field118 = SkeletonInit_GetBehaviorFlags(actor);
    saved->field11C = SkeletonInit_GetStatusFlags(actor);
    saved->field120 = SkeletonInit_GetDirection(actor);
    saved->field121 = SkeletonInit_GetState(actor);
    saved->field122 = SkeletonInit_GetPhase(actor);
    saved->field123 = SkeletonInit_GetBehavior(actor);
    saved->field124 = SkeletonInit_GetCurrentControl(actor);
    saved->field128 = SkeletonInit_GetControlKind(actor);
}
struct SkelCollider {
    u8 header[6];
    u16 flags;
};
static inline void SkeletonInit_ClearColliderFlags(struct SkelCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 SkeletonInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void SkeletonInit_SetBehaviorFlags(struct SkeletonActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void SkeletonInit_SetStatusFlags(struct SkeletonActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline void SkeletonInit_SetMotionFlags(struct SkeletonActor *actor, u16 mask)
{
    actor->field2C0 |= mask;
}
static inline u8 SkeletonInit_TestMotionFlags(struct SkeletonActor *actor, u32 mask)
{
    if (SkeletonInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 SkeletonInit_TestSkeletonFlags(struct SkeletonActor *actor, u32 mask)
{
    if (actor->field6DC & mask)
        return 1;
    return 0;
}
static inline u8 SkeletonInit_TestTargetingFlags(struct SkelTargeting *target, u32 mask)
{
    if (SkeletonInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void SkeletonInit_SetTargetingFlags(struct SkelTargeting *target, u8 mask)
{
    mask |= target->fieldEB;
    target->fieldEB = mask;
}
static inline void SkeletonInit_ClearControlFlags(struct SkeletonActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 SkeletonInit_IsInCurrentRegion(struct SkeletonActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *SkeletonInit_GetStandingControl(struct SkeletonActor *actor)
{
    return actor->field22C;
}
static inline void SkeletonInit_SetControl(struct SkeletonActor *actor, void *callback,
                                           u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    SkeletonInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void SkeletonInit_SetState(struct SkeletonActor *actor, u32 state, u32 phase,
                                         u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}
static inline void SkeletonInit_ResetFlags(void *object)
{
    struct SkeletonActor *actor = object;
    actor->field6DC = 0;
}
static inline void SkeletonInit_SetFlags(struct SkeletonActor *actor, u32 mask)
{
    actor->field6DC |= mask;
}
static inline void SkeletonInit_InitializeGate(struct SkelGate *gate, u32 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;
}

static inline void SkeletonInit_ReadTargeting(struct SkelTargeting *target)
{
    s32 nearRadius, flags, farRadius;
    if (Script_SeekToKeyword(0x65)) {
        nearRadius = SkeletonInit_Value(0x30);
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
    if (SkeletonInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);
}

static inline u16 SkeletonInit_RangeRadius(struct SkelRange *range)
{
    return range->field422;
}
static inline void SkeletonInit_ReadRange(struct SkelRange *range)
{
    s32 radius;
    if (Script_SeekToKeyword(0x6E)) {
        radius = SkeletonInit_Value(0);
        range->field420 = SkeletonInit_Value(0);
    } else {
        radius = 0;
        range->field420 = radius;
    }
    range->field422 = radius;
    range->field424 = SkeletonInit_RangeRadius(range) * SkeletonInit_RangeRadius(range);
    range->field42A = 0;
}

static inline u32 SkeletonInit_RoutePointX(struct SkelRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 SkeletonInit_RoutePointZ(struct SkelRoute *route)
{
    return (u16)route->point->z;
}
static inline void SkeletonInit_SnapToRoute(struct SkelPosition *position, struct SkelRoute *route)
{
    position->x = SkeletonInit_RoutePointX(route);
    position->z = SkeletonInit_RoutePointZ(route);
    position->y = SkeletonInit_Height(SkeletonInit_Cell(position), (u16 *)position);
}
static inline void SkeletonInit_DrawablePosition(struct SkeletonActor *actor)
{
    if (SkeletonInit_GetDrawableKind(actor) == 0)
        *(struct SkelPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct SkelPosition *)(actor->drawable + 0x40) = actor->body.position;
}
static inline void SkeletonInit_SetFirstAttack(struct SkeletonActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u16 SkeletonInit_KeywordHalfword(u32 keyword, s32 fallback)
{
    return Script_GetKeywordValue(keyword, fallback);
}

static inline u8 SkeletonInit_RouteId(struct SkelRoute *route)
{
    return route->routeId;
}
static inline u8 SkeletonInit_RouteParameter(struct SkelRoute *route)
{
    return route->parameterB;
}
static inline u8 SkeletonInit_RoutePointId(struct SkelRoute *route)
{
    return route->pointId;
}
static inline u8 SkeletonInit_RouteActiveBranch(struct SkelRoute *route)
{
    return route->activeBranch;
}
static inline u32 SkeletonInit_RouteAlternateBranch(struct SkelRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void SkeletonInit_BeginRoute(struct SkeletonBody *body, struct SkelRoute *route)
{
    SceneBody_StartRoute(body, route, SkeletonInit_RouteId(route),
                         SkeletonInit_RouteParameter(route), SkeletonInit_RoutePointId(route));
}
static inline s32 SkeletonInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void SkeletonInit_InstallIdle(struct SkeletonActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void SkeletonInit_InitializeHealth(struct SkeletonActor *arg0, u16 initialHealth,
                                                 s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    SkeletonInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = SkeletonInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;
}

static inline void SkeletonInit_SetInitialHealth(struct SkeletonActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 SkeletonInit_ReadRoute(struct SkeletonActor *arg0, struct SkeletonGroup *group,
                                         struct SkelRoute *rt)
{
    struct SkelPosition point;
    s32 pointId;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = SkeletonInit_Value(0);
        rt->parameterB = 0;
        pointId = SkeletonInit_Value(0);
        rt->pointId = pointId;
        rt->currentPoint = pointId;
        rt->branches[SkeletonInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[SkeletonInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : SkeletonInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, SkeletonInit_RouteId(rt),
                                  SkeletonInit_RoutePointId(rt));
        arg0->field2F4 = SkeletonInit_Cell(&point);
        point.y = arg0->field2BE =
            SkeletonInit_Height(SkeletonInit_GetTerrainCell(arg0), (u16 *)&point);
        SkeletonInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[SkeletonInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[SkeletonInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}
static inline void SkeletonInit_InitializeBody(void *object, u32 *fill)
{
    struct SkeletonActor *arg0 = object;
    struct SkeletonGroup *group = &arg0->group;
    struct SkeletonBody *body = &arg0->body;
    struct SkelPosition origin;
    s32 mode, i, secondLimit, firstLimit, gateDuration, identity;
    struct SkelRoute *rt;
    arg0->field2CC = 0;
    arg0->field2D0 = 0;
    arg0->field2B4 = 0;
    arg0->field2E8 = 8;
    arg0->field2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    SkeletonInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        i = 0;
        while (1) {
            SkeletonInit_SetFirstAttack(arg0, i, SkeletonInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = SkeletonInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = SkeletonInit_Value(0);
            arg0->attacks.parameter[i] = SkeletonInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = SkeletonInit_Value(0x12C);
        secondLimit = SkeletonInit_Value(0x12C);
        gateDuration = SkeletonInit_Value(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    SkeletonInit_InitializeGate(&group->gate, gateDuration);
    SkeletonInit_ReadRange(&group->range);
    SkeletonInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        SkeletonInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = SkeletonInit_KeywordHalfword(0x46, 0);
    arg0->field1EC = SkeletonInit_KeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        SkeletonInit_SetInitialHealth(arg0, SkeletonInit_ReadHalfword(0));
        group->field5B6 = SkeletonInit_Half(SkeletonInit_GetInitialHealth(arg0));
        arg0->field9E = SkeletonInit_ReadByte(0);
        arg0->field9F = SkeletonInit_ReadByte(0);
        arg0->fieldA0 = SkeletonInit_ReadByte(0);
        arg0->fieldA2 = SkeletonInit_ReadHalfword(0);
        arg0->fieldA4 = SkeletonInit_ReadHalfword(0);
    }

    SkeletonInit_InitializeHealth(arg0, SkeletonInit_GetInitialHealth(arg0), firstLimit,
                                  secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = SkeletonInit_Value(0);
        arg0->fieldA1 = SkeletonInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->field9A = identity;
    if (Script_GetKeywordValue(0x48, 0))
        SkeletonInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        SkeletonInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (SkeletonInit_ReadRoute(arg0, group, rt)) {
        SkeletonInit_SnapToRoute(&arg0->body.position, rt);
        SkeletonInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (SkeletonInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, SkeletonInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (SkeletonInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct SkelWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void SkeletonInit_SetWidgetScriptPalette(struct SkelWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}
static inline void SkeletonInit_ReadWidgetRelativePalette(struct SkelWidget *widget)
{
    widget->paletteBank = Script_GetPc() ? Script_GetValue() + 0x12B : 0x12B;
}
static inline void SkeletonInit_SetMotionControl(struct SkeletonActor *actor, void *callback,
                                                 u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void SkeletonInit_ClearMotion(void *object)
{
    struct SkeletonActor *actor = object;
    SkeletonInit_SetMotionControl(actor, 0, 0);
    actor->field114 = 0;
}
static inline void SkeletonInit_ReadPalette(struct SkeletonActor *actor, u32 base, u32 alternate)
{
    s32 palette;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = SkeletonInit_GetPaletteOffset(actor) + base;
    palette = SkeletonInit_GetPaletteBase(actor) + SkeletonInit_GetPaletteOffset(actor);
    if (SkeletonInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (SkeletonInit_GetDrawableKind(actor) == 1)
        SkeletonInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                            SceneActorScript_GetManagerField62());
}

static inline void SkeletonInit_InstallPrimary(struct SkeletonActor *arg0, s32 (*cb0)(void *),
                                               u16 (*cb1)(void *), void (*cb2)(void *),
                                               void (*cb3)(void *), void (*cb4)(void *),
                                               void (*hidden)(void *), void (*cb5)(void *),
                                               void (*cb6)(void *), u32 (*defaultReaction)(void),
                                               u8 (*cb270)(void *), void *targetQuery, void *damage,
                                               void *contact)
{
    SkeletonInit_SetBehaviorFlags(arg0, 0x2000);
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
    SkeletonInit_ClearMotion(arg0);
}

static inline void SkeletonInit_InitializeGroup(struct SkeletonGroup *group, void *firstBounds,
                                                void *secondBounds, void *downed,
                                                void *returnToRoute, void *recover, void *animate,
                                                struct SkeletonBody *body)
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
    ProximityVolume_TranslateBounds(firstBounds, SkeletonInit_BodyPosition(body));
}

static inline void SkeletonInit_SetRouteReset(struct SkeletonActor *actor, void *callback)
{
    actor->field248 = callback;
}
static inline void SkeletonInit_SetReactionGate(struct SkeletonActor *actor, void *callback)
{
    actor->field23C = callback;
}

static inline void SkeletonInit_InstallContactDefaults(struct SkeletonActor *arg0)
{
    arg0->field268 = Skeleton_ReactToContact;
    arg0->field250 = gUnk_086060FC;
    arg0->field238 = SceneActorReaction_SinkThenDefeat;
}

static inline void SkeletonInit_EnableInitialFlags(struct SkeletonActor *arg0)
{
    SkeletonInit_SetTargetingFlags(&arg0->targeting, 1);
    SkeletonInit_SetBehaviorFlags(arg0, 0x1600000);
    SkeletonInit_SetStatusFlags(arg0, 0x8000008);
    SkeletonInit_SetMotionFlags(arg0, 0x80);
}

static inline void SkeletonInit_ReadInitialMode(struct SkeletonActor *arg0,
                                                struct SkeletonGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.field5CC = 0x21;
    if (i != 0) {
        SkeletonInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            SkeletonInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void SkeletonInit_InitializeRegion(struct SkeletonActor *arg0)
{
    if (SkeletonInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline void SkeletonInit_InitializeReceiver(struct SkeletonActor *arg0)
{
    arg0->field304 = 0;
    SceneActorRegistry_RegisterCategoryReceiver(arg0);
}

static inline void SkeletonInit_InstallCompletionCallbacks(struct SkeletonActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Skeleton_UpdateBehaviorCallbacks;
    arg0->field25C = Skeleton_Destroy;
}

static inline void SkeletonInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void SkeletonInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                                 struct SkelPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct SkeletonResourceBlock *)drawable = *(struct SkeletonResourceBlock *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}
static inline void SkeletonInit_SetFlagWord(u32 mask, u32 *field)
{
    *field |= mask;
}

s32 Skeleton_Init(void *object)
{
    struct SkeletonActor *arg0 = object;
    struct SkeletonActor *actor = object;
    struct SkeletonGroup *group = &arg0->group;
    struct SkeletonBody *body = &arg0->body;
    u32 zero;
    struct SkelPosition tmp;
    u32 flag;

    SkeletonInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    {
        SkeletonInit_SetCategory(actor, Script_GetKeywordValue(0x6B, 5));
        if (SkeletonInit_GetCategory(actor) == 7)
            SkeletonInit_SetFlagWord(0x40, &arg0->field6DC);
    }
    SceneActor_UpdateProfileForCategory(actor);

    if (SkeletonInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = SkeletonInit_Drawable(actor);
        SkeletonInit_InitializeSprite(heap, 0xAE52, 0);
        if (SkeletonInit_GetCategory(actor) == 0x11 || SkeletonInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = SkeletonInit_Drawable(actor);
        SkeletonInit_ClearPosition(&zero, &tmp);
        SkeletonInit_InitializeWidget(heap, 0xCB05, 0xAE52, &tmp);
        actor->field2B7 = 0xFF;
    }

    SkeletonInit_InitializeBody(arg0, &zero);
    SceneActorReaction_ParseOptions(actor);
    SceneActorScript_ReadPParameters(actor);
    SceneActorScript_ReadCallScriptConfig(actor);

    SkeletonInit_ReadPalette(actor, 0x13C, 0x146);
    SceneActorControl_InitBehaviorColliders(actor);

    SkeletonInit_InstallPrimary(actor, Skeleton_Update, SceneActorReaction_SelectTileParameter,
                                sub_080BAE04, Skeleton_UpdateInactiveState,
                                Skeleton_UpdateIdleState, TentacleBody_UpdateReactionTransition,
                                Skeleton_ConvergeDisplacedPosition, Skeleton_UpdateDefeat,
                                SceneActorReaction_OnEnemyDefault, sub_0807F81C,
                                SceneActorTarget_ResolvePendingObjectSkeleton, sub_081363F4,
                                SceneActorPlacement_ResetStateThree);
    SkeletonInit_InitializeGroup(group, (u8 *)arg0 + 0x71C, (u8 *)arg0 + 0x740,
                                 Skeleton_UpdateDownedState, SceneActorReaction_ReturnToRouteEntry,
                                 SceneActorReaction_BeginTargetRecovery, 0, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    SkeletonInit_SetRouteReset(actor, Skeleton_ResetRouteReaction);
    group->field61C = 0;
    SkeletonInit_SetReactionGate(actor, SceneActorReaction_UpdateGate);
    group->field5EC = Skeleton_RestorePursuitRoute;
    SkeletonInit_InstallContactDefaults(actor);
    actor->field2E8 = 0;
    arg0->field718 = (s16)SkeletonInit_GetHealth(actor) >> 1;
    SkeletonInit_EnableInitialFlags(actor);
    SkeletonInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    SkeletonInit_ReadInitialMode(actor, group);
    flag = 0;
    if (SkeletonInit_TestMotionFlags(actor, 4)) {
        SkeletonInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }

    if (flag != 1) {
        SkeletonInit_SetState(actor, 0, 0, 0);
        SkeletonInit_SetControl(actor, SkeletonInit_GetStandingControl(actor), 0);
    }

    SkeletonInit_InitializeRegion(actor);
    if (SkeletonInit_TestMotionFlags(actor, 0x40))
        SkeletonInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !SkeletonInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);

    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (SkeletonInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);

    SkeletonInit_InitializeReceiver(actor);
    Skeleton_InitVariantMotion(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Skeleton_SetMovementControlPreset(actor);
    Skeleton_SetAttackControlTable(actor);
    Skeleton_SetReactionControlTable(actor);

    SkeletonInit_SaveControl(SkeletonInit_SavedControlOf(actor), actor);
    actor->saved.field130 = arg0->field6DC;

    if (SkeletonInit_TestSkeletonFlags(arg0, 0x800)) {
        SkeletonInit_SetControl(actor, Skeleton_BeginKnockedFlight, 0x3D);
        SkeletonInit_SetState(actor, 3, 0, 0);
        SkeletonInit_SetFlags(arg0, 0x10);
        SkeletonInit_SetMotionFlags(actor, 0x1800);
        if (Script_SeekToKeyword(0x70)) {
            actor->body.position.x = SkeletonInit_ReadHalfword(0);
            actor->body.position.y = SkeletonInit_ReadHalfword(0);
            actor->body.position.z = SkeletonInit_ReadHalfword(0);
            actor->body.direction = SkeletonInit_ReadByte(0);
            SkeletonInit_ReadWidgetRelativePalette((void *)(actor->drawable + 0x20));
            actor->field2F4 = SkeletonInit_Cell(SkeletonInit_BodyPosition(&actor->body));
            actor->field2BE = SkeletonInit_Height(SkeletonInit_GetTerrainCell(actor),
                                                  (u16 *)SkeletonInit_BodyPosition(&actor->body));
            actor->field2EA = actor->body.position.y - actor->field2BE;
            actor->body.position.y = actor->field2BE;
        }
    } else if (SkeletonInit_TestSkeletonFlags(arg0, 8)) {
        SkeletonInit_SetControl(actor, SceneActorReaction_TickHiddenTimeout, 0x39);
        SkeletonInit_SetState(actor, 3, 0, 0);
        SkeletonInit_SetMotionFlags(actor, 0x1800);
        SkeletonInit_SetFlags(arg0, 0x10);
    }
    if (SkeletonInit_GetCategory(actor) == 8)
        SkeletonInit_SetMotionFlags(actor, 0x800);

    SkeletonInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
