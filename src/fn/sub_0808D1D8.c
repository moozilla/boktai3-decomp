#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"

void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void Ghoul_BindProfile(void *);
void Vorn_ParseField624_0808B320(void *);
void Ghoul_Update(void);
u32 Bat_LookupAnimationEntry(void *);
void Ghoul_TurnTowardRoute(void);
void Ghoul_UpdateIdleState(void);
void Ghoul_UpdateStandingState(void);
void sub_08087A70(void);
void Bat_UpdateDefeat(void);
void Ghoul_UpdateHiddenRecovery(void);
void SceneActorReaction_OnEnemyDefault(void);
u8 sub_0807F81C(void *);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorReaction_SinkAndRemove(void);
void sub_081363F4(void);
void SceneActorPlacement_ResetStateThree(void);
void Ghoul_UpdateDyingState(void);
void Bat_ResumeSavedTarget(void);
void SceneActorReaction_BeginTargetRecovery(void);
void Bat_UpdateAnimationRequest(void);
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
void Ghoul_ResetRouteState(void);
void SceneActorControl_RefreshMotionRoute_0808A90C(void);
void Ghoul_ReactToContact(void);
void Ghoul_OnLifecycleEvent(void);
void Ghoul_ResumeContactRoute(void);
void Ghoul_OnMessage(void);
void Ghoul_Destroy(void);
void SceneActorScript_OnCommand(void);
void SceneActorControl_OnIdle(void);
void sub_08080BE0(void);
void Ghoul_InitCollision(void *);
void Ghoul_SetActionHandlers(void *);
void Ghoul_SetBehaviorHandlers(void *);
void Ghoul_SetSpecialHandlers(void *);
void Ghoul_SetBehaviorTable(void *);
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

struct GhoulInit32Block {
    u32 w[8];
};

void *Heap_Alloc(u32);
void *ScriptProc_Start(u32, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
extern const u8 gUnk_086108B0[];
extern const u8 gUnk_08606400[];
void AnimData_SelectPaletteBank(void *, s32);
void Anim_Step(void *, void *);
u8 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

extern u8 *gGameState;

struct GhoulInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct ContactPosition position;
    u8 unknown64[0x34];
};
struct GhoulInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct GhoulInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct GhoulInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct GhoulInitRoutePoint { s16 x, z; };
struct GhoulInitRoute {
    u32 unknown0, unknown4;
    struct GhoulInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct GhoulInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct GhoulInitGate gate;
    struct GhoulInitRange range;
    u8 unknown42C[0x10];
    void * field43C;
    u32 field440;
    void * field444;
    struct GhoulInitRoute route;
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
struct GhoulInitTargeting {
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
struct GhoulInitSavedControl {
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
struct GhoulInitActor {
    u8 unknown0[0x4];
    u8 * drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct GhoulInitBody body;
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
    struct GhoulInitAttacks attacks;
    struct GhoulInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct GhoulInitSavedControl saved;
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
    u8 (*field270)(void *);
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
    u8 unknown2F6[0x4];
    u16 field2FA;
    u8 unknown2FC[0x8];
    u32 field304;
    u8 unknown308[0xCC];
    struct GhoulInitGroup group;
    u16 field624, field626, field628;
    u8 unknown62A[0x14];
    u16 field63E;
    u8 field640;
    u8 unknown641[9];
    u16 field64A;
    u8 unknown64C[0x90];
    u32 field6DC;
    u8 unknown6E0[0x38];
    u32 field718;};

static inline u8 GhoulInit_GetDrawableKind(struct GhoulInitActor *actor)
{
    return actor->drawableKind;
}
static inline u8 GhoulInit_GetCategory(void *object)
{
    struct GhoulInitActor *actor = object;
    return actor->category;
}

static inline u16 GhoulInit_GetInitialHealth(struct GhoulInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 GhoulInit_GetPaletteBase(struct GhoulInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 GhoulInit_GetPaletteOffset(struct GhoulInitActor *actor)
{
    return actor->field1E6;
}

extern u8 *gUnknown_02000580;

extern const u8 gUnk_086060FC[];

static inline u32 GhoulInit_GetMotionFlags(struct GhoulInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 GhoulInit_GetHeightOffset(struct GhoulInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 GhoulInit_GetBehaviorFlags(struct GhoulInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 GhoulInit_GetStatusFlags(struct GhoulInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 GhoulInit_GetState(struct GhoulInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 GhoulInit_GetPhase(struct GhoulInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 GhoulInit_GetBehavior(struct GhoulInitActor *actor)
{
    return actor->field2AA;
}
static inline void * GhoulInit_GetCurrentControl(struct GhoulInitActor *actor)
{
    return actor->field21C;
}
static inline u8 GhoulInit_GetControlKind(struct GhoulInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 GhoulInit_GetDirection(struct GhoulInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 GhoulInit_GetTerrainCell(struct GhoulInitActor *actor)
{
    return actor->field2F4;
}
static inline u32 GhoulInit_GetTargetingFlags(struct GhoulInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct GhoulInitSavedControl *GhoulInit_SavedControlOf(struct GhoulInitActor *actor)
{
    struct GhoulInitSavedControl *saved = &actor->saved;
    return saved;
}
static inline struct ContactPosition *GhoulInit_BodyPosition(void *object)
{
    struct GhoulInitBody *body = object;
    struct ContactPosition *position = &body->position;
    return position;
}
static inline void GhoulInit_SaveControl(struct GhoulInitSavedControl *saved, struct GhoulInitActor *actor)
{
    saved->field12E = GhoulInit_GetMotionFlags(actor);
    saved->field12C = GhoulInit_GetHeightOffset(actor);
    saved->field118 = GhoulInit_GetBehaviorFlags(actor);
    saved->field11C = GhoulInit_GetStatusFlags(actor);
    saved->field120 = GhoulInit_GetDirection(actor);
    saved->field121 = GhoulInit_GetState(actor);
    saved->field122 = GhoulInit_GetPhase(actor);
    saved->field123 = GhoulInit_GetBehavior(actor);
    saved->field124 = GhoulInit_GetCurrentControl(actor);
    saved->field128 = GhoulInit_GetControlKind(actor);
}
struct GhoulInitCollider { u8 header[6]; u16 flags; };
static inline void GhoulInit_ClearColliderFlags(struct GhoulInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 GhoulInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void GhoulInit_SetBehaviorFlags(struct GhoulInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void GhoulInit_SetStatusFlags(struct GhoulInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline u8 GhoulInit_TestMotionFlags(struct GhoulInitActor *actor, u32 mask)
{
    if (GhoulInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 GhoulInit_TestTargetingFlags(struct GhoulInitTargeting *target, u32 mask)
{
    if (GhoulInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void GhoulInit_SetTargetingFlags(struct GhoulInitTargeting *target, u8 mask)
{
    mask |= target->fieldEB;
    target->fieldEB = mask;
}
static inline void GhoulInit_ClearControlFlags(struct GhoulInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 GhoulInit_IsInCurrentRegion(struct GhoulInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *GhoulInit_GetStandingControl(struct GhoulInitActor *actor)
{
    return actor->field22C;
}

static inline void GhoulInit_SetControl(struct GhoulInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    GhoulInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void GhoulInit_SetState(struct GhoulInitActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}

static inline void GhoulInit_InitializeGate(struct GhoulInitGate *gate, u32 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;

}

static inline void GhoulInit_ReadTargeting(struct GhoulInitTargeting *target)
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
    if (GhoulInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);

}

static inline u16 GhoulInit_RangeRadius(struct GhoulInitRange *range)
{
    return range->field422;
}
static inline void GhoulInit_ReadRange(struct GhoulInitRange *range)
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
    range->field424 = GhoulInit_RangeRadius(range) * GhoulInit_RangeRadius(range);
    range->field42A = 0;

}

static inline u32 GhoulInit_RoutePointX(struct GhoulInitRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 GhoulInit_RoutePointZ(struct GhoulInitRoute *route)
{
    return (u16)route->point->z;
}

static inline void GhoulInit_SnapToRoute(struct ContactPosition *position, struct GhoulInitRoute *route)
{
    position->x = GhoulInit_RoutePointX(route);
    position->z = GhoulInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void GhoulInit_DrawablePosition(struct GhoulInitActor *actor)
{
    if (GhoulInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 GhoulInit_RouteId(struct GhoulInitRoute *route)
{
    return route->routeId;
}
static inline u8 GhoulInit_RouteParameter(struct GhoulInitRoute *route)
{
    return route->parameterB;
}
static inline u8 GhoulInit_RoutePointId(struct GhoulInitRoute *route)
{
    return route->pointId;
}
static inline u8 GhoulInit_RouteActiveBranch(struct GhoulInitRoute *route)
{
    return route->activeBranch;
}

static inline s32 GhoulInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void GhoulInit_InstallIdle(struct GhoulInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void GhoulInit_InitializeHealth(struct GhoulInitActor *arg0,
    u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    GhoulInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = GhoulInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;

}

static inline void GhoulInit_SetInitialHealth(struct GhoulInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 GhoulInit_RouteAlternateBranch(struct GhoulInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void GhoulInit_BeginRoute(struct GhoulInitBody *body, struct GhoulInitRoute *route)
{
    SceneBody_StartRoute(body, route, GhoulInit_RouteId(route), GhoulInit_RouteParameter(route), GhoulInit_RoutePointId(route));
}

static inline void GhoulInit_SetFirstAttack(struct GhoulInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 GhoulInit_ReadRoute(struct GhoulInitActor *arg0, struct GhoulInitGroup *group, struct GhoulInitRoute *rt)
{
    struct ContactPosition point;
    s32 r1v;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = MonsterInit_ReadByte(0);
        rt->parameterB = 0;
        r1v = SV(0);
        rt->pointId = r1v;
        rt->currentPoint = r1v;
        rt->branches[GhoulInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[GhoulInit_RouteAlternateBranch(rt)] = Script_GetPc() ? Script_GetValue() : GhoulInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, GhoulInit_RouteId(rt), GhoulInit_RoutePointId(rt));
        arg0->field2F4 = MonsterInit_CellIndex(&point);
        point.y = arg0->field2BE = MonsterInit_TerrainHeight(GhoulInit_GetTerrainCell(arg0), &point);
        GhoulInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[GhoulInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[GhoulInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void GhoulInit_InitializeBody(void *object, u32 *fill)
{
    struct GhoulInitActor *arg0 = object;
    struct GhoulInitGroup *group = &arg0->group;
    struct GhoulInitBody *body = &arg0->body;
    struct ContactPosition origin;
    struct ContactPosition point;
    s32 mode, i, r4v, r6v, r5v, r2v, r1v;
    u16 initialHealth;
    struct GhoulInitRoute *rt;
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
            GhoulInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
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
    GhoulInit_InitializeGate(&group->gate, r2v);
    GhoulInit_ReadRange(&group->range);
    GhoulInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        GhoulInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        GhoulInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->field5B6 = GhoulInit_Half(GhoulInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    GhoulInit_InitializeHealth(arg0, GhoulInit_GetInitialHealth(arg0),
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
        GhoulInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        GhoulInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (GhoulInit_ReadRoute(arg0, group, rt)) {
        GhoulInit_SnapToRoute(&arg0->body.position, rt);
        GhoulInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (GhoulInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, GhoulInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (GhoulInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct GhoulInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void GhoulInit_SetWidgetScriptPalette(struct GhoulInitWidget *widget, s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void GhoulInit_SetMotionControl(struct GhoulInitActor *actor, void *callback, u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void GhoulInit_ClearMotion(void *object)
{
    struct GhoulInitActor *actor = object;
    u32 zero = 0;
    GhoulInit_SetMotionControl(actor, 0, 0);
    actor->field114 = zero;
}
static inline void GhoulInit_ReadPalette(struct GhoulInitActor *actor, u32 base, u32 alternate)
{
    s32 r1v;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = GhoulInit_GetPaletteOffset(actor) + base;
    r1v = GhoulInit_GetPaletteBase(actor) + GhoulInit_GetPaletteOffset(actor);
    if (GhoulInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (GhoulInit_GetDrawableKind(actor) == 1)
        GhoulInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20), SceneActorScript_GetManagerField62());
}

static inline void GhoulInit_InstallPrimary(struct GhoulInitActor *arg0,
    void *cb0,
    u32 (*cb1)(void *),
    void *cb2,
    void *cb3,
    void *cb4, void *hidden,
    void *cb5,
    void *cb6, void *defaultReaction, u8 (*cb270)(void *),
    void *targetQuery, void *damage, void *contact)
{
    GhoulInit_SetBehaviorFlags(arg0, 0x2000);
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
    GhoulInit_ClearMotion(arg0);
}

static inline void GhoulInit_InitializeGroup(struct GhoulInitGroup *group, void *firstBounds,
    void *secondBounds, void *downed, void *returnToRoute, void *recover,
    void *animate, struct GhoulInitBody *body)
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
    ProximityVolume_TranslateBounds(firstBounds, GhoulInit_BodyPosition(body));
}

static inline void GhoulInit_InstallRouteDefaults(struct GhoulInitActor *arg0)
{
    arg0->field248 = Ghoul_ResetRouteState;
    arg0->field23C = SceneActorControl_RefreshMotionRoute_0808A90C;
}

static inline void GhoulInit_ReadInitialMode(struct GhoulInitActor *arg0, struct GhoulInitGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.field5CC = 0x21;
    if (i != 0) {
        GhoulInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            GhoulInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void GhoulInit_InitializeRegion(struct GhoulInitActor *arg0)
{
    if (GhoulInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);

}

static inline void GhoulInit_InitializeReceiver(struct GhoulInitActor *arg0)
{
    arg0->field304 = 0;
    SceneActorRegistry_RegisterCategoryReceiver(arg0);
}

static inline void GhoulInit_InstallCompletionCallbacks(struct GhoulInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Ghoul_OnMessage;
    arg0->field25C = Ghoul_Destroy;

}

static inline void GhoulInit_InitializePaletteRange(struct GhoulInitActor *actor, u32 alternate, u32 current)
{
    actor->field1DC = GhoulInit_GetPaletteBase(actor) + alternate;
    actor->field1DE = GhoulInit_GetPaletteBase(actor) + current;
}

static inline u8 *GhoulInit_Drawable(struct GhoulInitActor *actor)
{
    return actor->drawable;
}

static inline void GhoulInit_SetCategory(struct GhoulInitActor *actor, u8 category)
{
    actor->category = category;
}

static inline void GhoulInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void GhoulInit_SetContactCallback(struct GhoulInitActor *actor, void *callback)
{
    actor->field268 = callback;
}

static inline void GhoulInit_InitializeWidget(u8 *drawable, u16 script, u16 resource, struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct GhoulInit32Block *)drawable = *(struct GhoulInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

s32 Ghoul_Init(void *object)
{
    struct GhoulInitActor *arg0 = object;
    struct GhoulInitActor *actor = object;
    struct GhoulInitGroup *group = &arg0->group;
    struct GhoulInitBody *body = &arg0->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 mode;

    arg0->field628 = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    GhoulInit_SetCategory(actor, 4);
    Ghoul_BindProfile(actor);
    Vorn_ParseField624_0808B320(actor);

    if (GhoulInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = GhoulInit_Drawable(actor);
        GhoulInit_InitializeSprite(heap, 0x9247, 0x1DF8);
        if (GhoulInit_GetCategory(actor) == 0x11 || GhoulInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = GhoulInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        GhoulInit_InitializeWidget(heap, 0xCB05, 0x9247, &tmp);
        actor->field2B7 = 0xFF;
    }
    GhoulInit_InitializeBody(arg0, &zero);
    if (Script_SeekToKeyword(0x50)) {
        arg0->field640 = MonsterInit_ReadByte(0x78);
        arg0->field64A = MonsterInit_ReadHalfword(0x78);
        arg0->field63E = MonsterInit_ReadHalfword(0x78);
    } else {
        arg0->field640 = 0x78;
        arg0->field64A = 0x78;
    }
    GhoulInit_ReadPalette(actor, 0x192, 0x192);
    GhoulInit_InitializePaletteRange(actor, 4, 0);
    Ghoul_InitCollision(actor);
    GhoulInit_InstallPrimary(actor, Ghoul_Update, Bat_LookupAnimationEntry,
        Ghoul_TurnTowardRoute, Ghoul_UpdateIdleState, Ghoul_UpdateStandingState, Ghoul_UpdateHiddenRecovery,
        sub_08087A70, Bat_UpdateDefeat, SceneActorReaction_OnEnemyDefault, sub_0807F81C,
        SceneActorTarget_ResolvePendingObject, sub_081363F4, SceneActorPlacement_ResetStateThree);
    GhoulInit_InitializeGroup(group, (u8 *)arg0 + 0x64C, (u8 *)arg0 + 0x670,
        Ghoul_UpdateDyingState, Bat_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Bat_UpdateAnimationRequest, body);
    SceneActorMovement_ResolveRoutePosition(arg0);
    GhoulInit_InstallRouteDefaults(actor);
    GhoulInit_SetContactCallback(actor, Ghoul_ReactToContact);
    group->field61C = (u32)Ghoul_OnLifecycleEvent;
    group->field5EC = Ghoul_ResumeContactRoute;
    GhoulInit_SetTargetingFlags(&actor->targeting, 1);
    GhoulInit_SetBehaviorFlags(actor, 0x1600000);
    GhoulInit_SetStatusFlags(actor, 0x8000000);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    GhoulInit_ReadInitialMode(actor, group);
    flag = 0;
    if (GhoulInit_TestMotionFlags(actor, 4)) {
        GhoulInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        GhoulInit_SetState(actor, 0, 0, 0);
        GhoulInit_SetControl(actor, GhoulInit_GetStandingControl(actor), 0);
    }
    GhoulInit_InitializeRegion(actor);
    if (GhoulInit_TestMotionFlags(actor, 0x40))
        GhoulInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !GhoulInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (GhoulInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    GhoulInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Ghoul_SetActionHandlers(actor);
    Ghoul_SetBehaviorHandlers(actor);
    Ghoul_SetSpecialHandlers(actor);
    Ghoul_SetBehaviorTable(actor);
    GhoulInit_SaveControl(GhoulInit_SavedControlOf(actor), actor);
    GhoulInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
