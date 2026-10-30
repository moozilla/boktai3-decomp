#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"

void Octopus_InitProfileId(void *);
void Octopus_Update(void);
void Octopus_UpdateDefeat(void);
void SceneActorControl_BeginSoundTransition_080EBC38(void);
void SceneActorControl_UpdateMotionPhase_080EBD0C(void);
void SceneActorCollision_InitCompactBody(void *);
void Octopus_InitControlPresets(void *);
void Octopus_BeginIdleAnimation(void *);
void Octopus_UpdateInteraction(void);
void Octopus_Destroy(void);
extern const u8 gUnk_0860635C[];

#define SK_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorReaction_OnEnemyDefault(void);
u8 sub_0807F81C(void *);
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

struct OctopusInit32Block {
    u32 w[8];
};

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
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

struct OctopusInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct ContactPosition position;
    u8 unknown64[0x34];
};
struct OctopusInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct OctopusInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct OctopusInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct OctopusInitRoutePoint { s16 x, z; };
struct OctopusInitRoute {
    u32 unknown0, unknown4;
    struct OctopusInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct OctopusInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct OctopusInitGate gate;
    struct OctopusInitRange range;
    u8 unknown42C[0x10];
    void * field43C;
    u32 field440;
    void * field444;
    struct OctopusInitRoute route;
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
struct OctopusInitTargeting {
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
struct OctopusInitSavedControl {
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
struct OctopusInitActor {
    u8 unknown0[0x4];
    u8 * drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct OctopusInitBody body;
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
    struct OctopusInitAttacks attacks;
    struct OctopusInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct OctopusInitSavedControl saved;
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
    struct OctopusInitGroup group;
    u8 unknown624[8];
    u32 captureFlags;
    u8 unknown630[6];
    u16 field636;
    u8 unknown638;
    u8 field639;
    u8 field63A;
    u8 unknown63B;
};

static inline u8 OctopusInit_GetDrawableKind(struct OctopusInitActor *actor)
{
    return actor->drawableKind;
}
static inline u8 OctopusInit_GetCategory(void *object)
{
    struct OctopusInitActor *actor = object;
    return actor->category;
}

static inline u16 OctopusInit_GetInitialHealth(struct OctopusInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 OctopusInit_GetPaletteBase(struct OctopusInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 OctopusInit_GetPaletteOffset(struct OctopusInitActor *actor)
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

static inline u32 OctopusInit_GetMotionFlags(struct OctopusInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 OctopusInit_GetHeightOffset(struct OctopusInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 OctopusInit_GetBehaviorFlags(struct OctopusInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 OctopusInit_GetStatusFlags(struct OctopusInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 OctopusInit_GetState(struct OctopusInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 OctopusInit_GetPhase(struct OctopusInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 OctopusInit_GetBehavior(struct OctopusInitActor *actor)
{
    return actor->field2AA;
}
static inline void * OctopusInit_GetCurrentControl(struct OctopusInitActor *actor)
{
    return actor->field21C;
}
static inline u8 OctopusInit_GetControlKind(struct OctopusInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 OctopusInit_GetDirection(struct OctopusInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 OctopusInit_GetTerrainCell(struct OctopusInitActor *actor)
{
    return actor->field2F4;
}
static inline u32 OctopusInit_GetTargetingFlags(struct OctopusInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct OctopusInitSavedControl *OctopusInit_SavedControlOf(struct OctopusInitActor *actor)
{
    struct OctopusInitSavedControl *saved = &actor->saved;
    return saved;
}

static inline void OctopusInit_SaveControl(struct OctopusInitSavedControl *saved, struct OctopusInitActor *actor)
{
    saved->field12E = OctopusInit_GetMotionFlags(actor);
    saved->field12C = OctopusInit_GetHeightOffset(actor);
    saved->field118 = OctopusInit_GetBehaviorFlags(actor);
    saved->field11C = OctopusInit_GetStatusFlags(actor);
    saved->field120 = OctopusInit_GetDirection(actor);
    saved->field121 = OctopusInit_GetState(actor);
    saved->field122 = OctopusInit_GetPhase(actor);
    saved->field123 = OctopusInit_GetBehavior(actor);
    saved->field124 = OctopusInit_GetCurrentControl(actor);
    saved->field128 = OctopusInit_GetControlKind(actor);
}
struct OctopusInitCollider { u8 header[6]; u16 flags; };
static inline void OctopusInit_ClearColliderFlags(struct OctopusInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 OctopusInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void OctopusInit_SetBehaviorFlags(struct OctopusInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void OctopusInit_SetStatusFlags(struct OctopusInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline void OctopusInit_SetMotionFlags(struct OctopusInitActor *actor, u16 mask)
{
    actor->field2C0 |= mask;
}
static inline u8 OctopusInit_TestMotionFlags(struct OctopusInitActor *actor, u32 mask)
{
    if (OctopusInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 OctopusInit_TestTargetingFlags(struct OctopusInitTargeting *target, u32 mask)
{
    if (OctopusInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void OctopusInit_ClearControlFlags(struct OctopusInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 OctopusInit_IsInCurrentRegion(struct OctopusInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *OctopusInit_GetStandingControl(struct OctopusInitActor *actor)
{
    return actor->field22C;
}

static inline void OctopusInit_SetControl(struct OctopusInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    OctopusInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void OctopusInit_SetState(struct OctopusInitActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}

static inline void OctopusInit_InitializeGate(struct OctopusInitGate *gate, u32 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;

}

static inline void OctopusInit_ReadTargeting(struct OctopusInitTargeting *target)
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
    if (OctopusInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);

}

static inline u16 OctopusInit_RangeRadius(struct OctopusInitRange *range)
{
    return range->field422;
}
static inline void OctopusInit_ReadRange(struct OctopusInitRange *range)
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
    range->field424 = OctopusInit_RangeRadius(range) * OctopusInit_RangeRadius(range);
    range->field42A = 0;

}

static inline u32 OctopusInit_RoutePointX(struct OctopusInitRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 OctopusInit_RoutePointZ(struct OctopusInitRoute *route)
{
    return (u16)route->point->z;
}

static inline void OctopusInit_SnapToRoute(struct ContactPosition *position, struct OctopusInitRoute *route)
{
    position->x = OctopusInit_RoutePointX(route);
    position->z = OctopusInit_RoutePointZ(route);
    position->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(position), position);
}
static inline void OctopusInit_DrawablePosition(struct OctopusInitActor *actor)
{
    if (OctopusInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 OctopusInit_RouteId(struct OctopusInitRoute *route)
{
    return route->routeId;
}
static inline u8 OctopusInit_RouteParameter(struct OctopusInitRoute *route)
{
    return route->parameterB;
}
static inline u8 OctopusInit_RoutePointId(struct OctopusInitRoute *route)
{
    return route->pointId;
}
static inline u8 OctopusInit_RouteActiveBranch(struct OctopusInitRoute *route)
{
    return route->activeBranch;
}

static inline s32 OctopusInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void OctopusInit_InstallIdle(struct OctopusInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void OctopusInit_InitializeHealth(struct OctopusInitActor *arg0,
    u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    OctopusInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = OctopusInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;

}

static inline void OctopusInit_SetInitialHealth(struct OctopusInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 OctopusInit_RouteAlternateBranch(struct OctopusInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void OctopusInit_BeginRoute(struct OctopusInitBody *body, struct OctopusInitRoute *route)
{
    SceneBody_StartRoute(body, route, OctopusInit_RouteId(route), OctopusInit_RouteParameter(route), OctopusInit_RoutePointId(route));
}

static inline void OctopusInit_SetFirstAttack(struct OctopusInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 OctopusInit_ReadRoute(struct OctopusInitActor *arg0, struct OctopusInitGroup *group, struct OctopusInitRoute *rt)
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
        rt->branches[OctopusInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[OctopusInit_RouteAlternateBranch(rt)] = Script_GetPc() ? Script_GetValue() : OctopusInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, OctopusInit_RouteId(rt), OctopusInit_RoutePointId(rt));
        arg0->field2F4 = MonsterInit_CellIndex(&point);
        point.y = arg0->field2BE = MonsterInit_TerrainHeight(OctopusInit_GetTerrainCell(arg0), &point);
        OctopusInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[OctopusInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[OctopusInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void OctopusInit_InitializeBody(void *object, u32 *fill)
{
    struct OctopusInitActor *arg0 = object;
    struct OctopusInitGroup *group = &arg0->group;
    struct OctopusInitBody *body = &arg0->body;
    struct ContactPosition origin;
    struct ContactPosition point;
    s32 mode, i, r4v, r6v, r5v, r2v, r1v;
    u16 initialHealth;
    struct OctopusInitRoute *rt;
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
            OctopusInit_SetFirstAttack(arg0, i, MonsterInit_ReadHalfword(0x18));
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
    OctopusInit_InitializeGate(&group->gate, r2v);
    OctopusInit_ReadRange(&group->range);
    OctopusInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        OctopusInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        OctopusInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->field5B6 = OctopusInit_Half(OctopusInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    OctopusInit_InitializeHealth(arg0, OctopusInit_GetInitialHealth(arg0),
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
        OctopusInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        OctopusInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (OctopusInit_ReadRoute(arg0, group, rt)) {
        OctopusInit_SnapToRoute(&arg0->body.position, rt);
        OctopusInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (OctopusInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, OctopusInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (OctopusInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct OctopusInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void OctopusInit_SetWidgetScriptPalette(struct OctopusInitWidget *widget, s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void OctopusInit_SetMotionControl(struct OctopusInitActor *actor, void *callback, u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void OctopusInit_ClearMotion(void *object)
{
    struct OctopusInitActor *actor = object;
    u32 zero = 0;
    OctopusInit_SetMotionControl(actor, 0, 0);
    actor->field114 = zero;
}
static inline void OctopusInit_ReadPalette(struct OctopusInitActor *actor, u32 base, u32 alternate)
{
    s32 r1v;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = OctopusInit_GetPaletteOffset(actor) + base;
    r1v = OctopusInit_GetPaletteBase(actor) + OctopusInit_GetPaletteOffset(actor);
    if (OctopusInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (OctopusInit_GetDrawableKind(actor) == 1)
        OctopusInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20), SceneActorScript_GetManagerField62());
}

static inline void OctopusInit_InstallPrimary(struct OctopusInitActor *arg0,
    void *cb0,
    u32 (*cb1)(void *),
    void *cb2,
    void *cb3,
    void *cb4, void *hidden,
    void *cb5,
    void *cb6, void *defaultReaction, void *cb270,
    void *targetQuery, void *damage, void *contact)
{
    OctopusInit_SetBehaviorFlags(arg0, 0x2000);
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
    OctopusInit_ClearMotion(arg0);
}

static inline void OctopusInit_InitializeGroup(struct OctopusInitGroup *group, void *firstBounds,
    void *secondBounds, void *downed, void *returnToRoute, void *recover,
    void *animate)
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
}

static inline void OctopusInit_InitializeRegion(struct OctopusInitActor *arg0)
{
    if (OctopusInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);

}

static inline void OctopusInit_InstallCompletionCallbacks(struct OctopusInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Octopus_UpdateInteraction;
    arg0->field25C = Octopus_Destroy;

}

static inline u8 *OctopusInit_Drawable(struct OctopusInitActor *actor)
{
    return actor->drawable;
}

static inline void OctopusInit_SetCategory(void *object, u8 category)
{
    struct OctopusInitActor *actor = object;
    actor->category = category;
}

static inline void OctopusInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void OctopusInit_InstallRoutes(struct OctopusInitActor *actor)
{
    actor->field248 = SceneActorControl_UpdateMotionPhase_080EBD0C;
    actor->field250 = gUnk_0860635C;
}
static inline void OctopusInit_ClearReceiver(struct OctopusInitActor *actor)
{
    actor->field304 = 0;
}

static inline void OctopusInit_InitializeWidget(u8 *drawable, u16 script, u16 resource, struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct OctopusInit32Block *)drawable = *(struct OctopusInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void OctopusInit_ResetCapture(void *object)
{
    struct OctopusInitActor *actor = object;
    actor->captureFlags = 0;
}

s32 Octopus_Init(void *object)
{
    struct OctopusInitActor *arg0 = object;
    struct OctopusInitActor *actor = object;
    struct OctopusInitGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    OctopusInit_ResetCapture(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    OctopusInit_SetCategory(actor, 14);
    Octopus_InitProfileId(actor);
    if (OctopusInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = OctopusInit_Drawable(actor);
        OctopusInit_InitializeSprite(heap, 0xD291, 0xDCEB);
        if (OctopusInit_GetCategory(actor) == 0x11 || OctopusInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = OctopusInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        OctopusInit_InitializeWidget(heap, 0xCB05, 0xD291, &tmp);
        actor->field2B7 = 0xFF;
    }
    OctopusInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        arg0->field636 = MonsterInit_ReadHalfword(0);
        arg0->field63A = MonsterInit_ReadByte(0);
        arg0->field639 = MonsterInit_ReadByte(0);
    }
    actor->body.position.x += 0x20;
    actor->body.position.z += 0x20;
    OctopusInit_DrawablePosition(actor);
    OctopusInit_ReadPalette(actor, 0x248, 0x192);
    SceneActorCollision_InitCompactBody(actor);
    OctopusInit_InstallPrimary(actor, Octopus_Update, 0, 0, 0, 0, 0,
        0, Octopus_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0, 0,
        sub_081363F4, SceneActorControl_BeginSoundTransition_080EBC38);
    OctopusInit_InitializeGroup(group, 0, 0, 0, 0, 0, 0);
    OctopusInit_InstallRoutes(actor);
    OctopusInit_SetBehaviorFlags(actor, 0x200);
    OctopusInit_SetStatusFlags(actor, 0x408);
    OctopusInit_SetMotionFlags(actor, 0x40);
    if (Script_GetKeywordValue('E', 0))
        OctopusInit_SetStatusFlags(actor, 0x2000000);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (OctopusInit_TestMotionFlags(actor, 4)) {
        OctopusInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        OctopusInit_SetState(actor, 0, 0, 0);
        OctopusInit_SetControl(actor, OctopusInit_GetStandingControl(actor), 0);
    }
    OctopusInit_InitializeRegion(actor);
    if (OctopusInit_TestMotionFlags(actor, 0x40))
        OctopusInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !OctopusInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (OctopusInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    OctopusInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Octopus_InitControlPresets(actor);
    Octopus_BeginIdleAnimation(actor);
    OctopusInit_SaveControl(OctopusInit_SavedControlOf(actor), actor);
    OctopusInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
