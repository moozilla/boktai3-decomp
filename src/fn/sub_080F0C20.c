#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
void SceneActorReaction_PlayHiddenCue(void);

void Ivy_InitProfileId(void *);
void Ivy_Update(void);
void Ivy_UpdateDefeat(void);
void SceneActorControl_BeginSoundTransition(void);
void Ivy_TickDrawable(void);
void SceneActor_InitCollision5005(void *);
void Ivy_InitControlPresets(void *);
void SceneActor_BeginReaction22(void *);
void Ivy_UpdateInteraction(void);
void Ivy_Destroy(void);
extern const u8 gUnk_08606400[];

#define SK_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void CpuSet(const void *, void *, u32);
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

s32 Script_GetValue(void);
u32 Script_GetPc(void);
static inline s32 IvyInit_Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
#define SV(d) IvyInit_Value(d)

struct IvyInit32Block {
    u32 w[8];
};

s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
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

struct IvyInitPosition { s16 x, y, z, pad; };
struct IvyInitMap { u32 header; u8 *cells; u8 unknown8[0x1C]; u16 rows[1]; };
static inline void IvyInit_ClearPosition(u32 *fill, void *object)
{
    struct IvyInitPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}
static inline u16 IvyInit_PositionX(const u16 *pos)
{
    return pos[0];
}
static inline u16 IvyInit_PositionZ(const u16 *pos)
{
    return pos[2];
}
static inline s32 IvyInit_Cell(u16 *pos)
{
    s32 x = (s16)IvyInit_PositionX(pos) >> 8;
    s32 z = (s16)IvyInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct IvyInitMap *)gTerrainMap)->rows[z] + x;
}
static inline u8 IvyInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}

static inline u32 IvyInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}

static inline u32 IvyInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 attributes, shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct IvyInitMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    shape = IvyInit_TerrainShape(p);
    height = IvyInit_TerrainBaseHeight(p);
    switch (shape) {
    case 1: height -= ((u8 *)pos)[4]; break;
    case 2: height -= ((u8 *)pos)[0]; break;
    }
    return height;
}

struct IvyInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct IvyInitPosition position;
    u8 unknown64[0x34];
};
struct IvyInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct IvyInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct IvyInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct IvyInitRoutePoint { s16 x, z; };
struct IvyInitRoute {
    u32 unknown0, unknown4;
    struct IvyInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct IvyInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct IvyInitGate gate;
    struct IvyInitRange range;
    u8 unknown42C[0x10];
    void * field43C;
    u32 field440;
    void * field444;
    struct IvyInitRoute route;
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
struct IvyInitTargeting {
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
struct IvyInitSavedControl {
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
struct IvyInitActor {
    u8 unknown0[0x4];
    u8 * drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct IvyInitBody body;
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
    struct IvyInitAttacks attacks;
    struct IvyInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct IvyInitSavedControl saved;
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
    struct IvyInitGroup group;
    u8 unknown624[8];
    u32 captureFlags;
    u8 unknown630[6];
    u16 field636;
    u32 field638;
    u8 unknown63C;
    u8 field63D;
    u8 field63E;
    u8 unknown63F;
};

static inline u8 IvyInit_GetDrawableKind(struct IvyInitActor *actor)
{
    return actor->drawableKind;
}
static inline u8 IvyInit_GetCategory(void *object)
{
    struct IvyInitActor *actor = object;
    return actor->category;
}

static inline u16 IvyInit_GetInitialHealth(struct IvyInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 IvyInit_GetPaletteBase(struct IvyInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 IvyInit_GetPaletteOffset(struct IvyInitActor *actor)
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

static inline u32 IvyInit_GetMotionFlags(struct IvyInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 IvyInit_GetHeightOffset(struct IvyInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 IvyInit_GetBehaviorFlags(struct IvyInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 IvyInit_GetStatusFlags(struct IvyInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 IvyInit_GetState(struct IvyInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 IvyInit_GetPhase(struct IvyInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 IvyInit_GetBehavior(struct IvyInitActor *actor)
{
    return actor->field2AA;
}
static inline void * IvyInit_GetCurrentControl(struct IvyInitActor *actor)
{
    return actor->field21C;
}
static inline u8 IvyInit_GetControlKind(struct IvyInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 IvyInit_GetDirection(struct IvyInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 IvyInit_GetTerrainCell(struct IvyInitActor *actor)
{
    return actor->field2F4;
}
static inline u32 IvyInit_GetTargetingFlags(struct IvyInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct IvyInitSavedControl *IvyInit_SavedControlOf(struct IvyInitActor *actor)
{
    struct IvyInitSavedControl *saved = &actor->saved;
    return saved;
}

static inline void IvyInit_SaveControl(struct IvyInitSavedControl *saved, struct IvyInitActor *actor)
{
    saved->field12E = IvyInit_GetMotionFlags(actor);
    saved->field12C = IvyInit_GetHeightOffset(actor);
    saved->field118 = IvyInit_GetBehaviorFlags(actor);
    saved->field11C = IvyInit_GetStatusFlags(actor);
    saved->field120 = IvyInit_GetDirection(actor);
    saved->field121 = IvyInit_GetState(actor);
    saved->field122 = IvyInit_GetPhase(actor);
    saved->field123 = IvyInit_GetBehavior(actor);
    saved->field124 = IvyInit_GetCurrentControl(actor);
    saved->field128 = IvyInit_GetControlKind(actor);
}
struct IvyInitCollider { u8 header[6]; u16 flags; };
static inline void IvyInit_ClearColliderFlags(struct IvyInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 IvyInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void IvyInit_SetBehaviorFlags(struct IvyInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void IvyInit_SetStatusFlags(struct IvyInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline void IvyInit_SetMotionFlags(struct IvyInitActor *actor, u16 mask)
{
    actor->field2C0 |= mask;
}
static inline u8 IvyInit_TestMotionFlags(struct IvyInitActor *actor, u32 mask)
{
    if (IvyInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 IvyInit_TestTargetingFlags(struct IvyInitTargeting *target, u32 mask)
{
    if (IvyInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void IvyInit_ClearControlFlags(struct IvyInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 IvyInit_IsInCurrentRegion(struct IvyInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *IvyInit_GetStandingControl(struct IvyInitActor *actor)
{
    return actor->field22C;
}

static inline void IvyInit_SetControl(struct IvyInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    IvyInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void IvyInit_SetState(struct IvyInitActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}

static inline void IvyInit_ResetState(struct IvyInitActor *actor, u32 behavior)
{
    IvyInit_SetState(actor, 0, 0, behavior);
}

static inline void IvyInit_InitializeGate(struct IvyInitGate *gate, u32 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;

}

static inline void IvyInit_ReadTargeting(struct IvyInitTargeting *target)
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
    if (IvyInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);

}

static inline u16 IvyInit_RangeRadius(struct IvyInitRange *range)
{
    return range->field422;
}
static inline void IvyInit_ReadRange(struct IvyInitRange *range)
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
    range->field424 = IvyInit_RangeRadius(range) * IvyInit_RangeRadius(range);
    range->field42A = 0;

}

static inline u32 IvyInit_RoutePointX(struct IvyInitRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 IvyInit_RoutePointZ(struct IvyInitRoute *route)
{
    return (u16)route->point->z;
}

static inline void IvyInit_SnapToRoute(struct IvyInitPosition *position, struct IvyInitRoute *route)
{
    position->x = IvyInit_RoutePointX(route);
    position->z = IvyInit_RoutePointZ(route);
    position->y = IvyInit_Height(IvyInit_Cell(position), (u16 *)position);
}
static inline void IvyInit_DrawablePosition(struct IvyInitActor *actor)
{
    if (IvyInit_GetDrawableKind(actor) == 0)
        *(struct IvyInitPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct IvyInitPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 IvyInit_RouteId(struct IvyInitRoute *route)
{
    return route->routeId;
}
static inline u8 IvyInit_RouteParameter(struct IvyInitRoute *route)
{
    return route->parameterB;
}
static inline u8 IvyInit_RoutePointId(struct IvyInitRoute *route)
{
    return route->pointId;
}
static inline u8 IvyInit_RouteActiveBranch(struct IvyInitRoute *route)
{
    return route->activeBranch;
}
static inline u16 IvyInit_ReadKeywordHalfword(u32 keyword, s32 fallback)
{
    return Script_GetKeywordValue(keyword, fallback);
}

static inline s32 IvyInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void IvyInit_InstallIdle(struct IvyInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void IvyInit_InitializeHealth(struct IvyInitActor *arg0,
    u16 initialHealth, s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    IvyInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = IvyInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;

}

static inline void IvyInit_SetInitialHealth(struct IvyInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 IvyInit_RouteAlternateBranch(struct IvyInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void IvyInit_BeginRoute(struct IvyInitBody *body, struct IvyInitRoute *route)
{
    SceneBody_StartRoute(body, route, IvyInit_RouteId(route), IvyInit_RouteParameter(route), IvyInit_RoutePointId(route));
}

static inline u8 IvyInit_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

static inline u16 IvyInit_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline void IvyInit_SetFirstAttack(struct IvyInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 IvyInit_ReadRoute(struct IvyInitActor *arg0, struct IvyInitGroup *group, struct IvyInitRoute *rt)
{
    struct IvyInitPosition point;
    s32 r1v;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = SV(0);
        rt->parameterB = 0;
        r1v = SV(0);
        rt->pointId = r1v;
        rt->currentPoint = r1v;
        rt->branches[IvyInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[IvyInit_RouteAlternateBranch(rt)] = Script_GetPc() ? Script_GetValue() : IvyInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, IvyInit_RouteId(rt), IvyInit_RoutePointId(rt));
        arg0->field2F4 = IvyInit_Cell(&point);
        point.y = arg0->field2BE = IvyInit_Height(IvyInit_GetTerrainCell(arg0), (u16 *)&point);
        IvyInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[IvyInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[IvyInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void IvyInit_InitializeBody(void *object, u32 *fill)
{
    struct IvyInitActor *arg0 = object;
    struct IvyInitGroup *group = &arg0->group;
    struct IvyInitBody *body = &arg0->body;
    struct IvyInitPosition origin;
    struct IvyInitPosition point;
    s32 mode, i, r4v, r6v, r5v, r2v, r1v;
    u16 initialHealth;
    struct IvyInitRoute *rt;
    arg0->field2CC = 0;
    arg0->field2D0 = 0;
    arg0->field2B4 = 0;
    arg0->field2E8 = 8;
    arg0->field2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    IvyInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        i = 0;
        while (1) {
            IvyInit_SetFirstAttack(arg0, i, IvyInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = IvyInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = SV(0);
            arg0->attacks.parameter[i] = IvyInit_ReadHalfword(0);
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
    IvyInit_InitializeGate(&group->gate, r2v);
    IvyInit_ReadRange(&group->range);
    IvyInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        IvyInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = IvyInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = IvyInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        IvyInit_SetInitialHealth(arg0, IvyInit_ReadHalfword(0));
        group->field5B6 = IvyInit_Half(IvyInit_GetInitialHealth(arg0));
        arg0->field9E = IvyInit_ReadByte(0);
        arg0->field9F = IvyInit_ReadByte(0);
        arg0->fieldA0 = IvyInit_ReadByte(0);
        arg0->fieldA2 = IvyInit_ReadHalfword(0);
        arg0->fieldA4 = IvyInit_ReadHalfword(0);
    }

    IvyInit_InitializeHealth(arg0, IvyInit_GetInitialHealth(arg0),
        r5v, r6v);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = SV(0);
        arg0->fieldA1 = IvyInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    r1v = (s16)Script_GetKeywordValue(0x56, -1);
    if (r1v < 0)
        r1v = 0xFFFF;
    arg0->field9A = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        IvyInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        IvyInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (IvyInit_ReadRoute(arg0, group, rt)) {
        IvyInit_SnapToRoute(&arg0->body.position, rt);
        IvyInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (IvyInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, IvyInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (IvyInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct IvyInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void IvyInit_SetWidgetScriptPalette(struct IvyInitWidget *widget, s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void IvyInit_SetMotionControl(struct IvyInitActor *actor, void *callback, u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void IvyInit_ClearMotion(void *object)
{
    struct IvyInitActor *actor = object;
    u32 zero = 0;
    IvyInit_SetMotionControl(actor, 0, 0);
    actor->field114 = zero;
}
static inline void IvyInit_ReadPalette(struct IvyInitActor *actor, u32 base, u32 alternate)
{
    s32 r1v;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = IvyInit_GetPaletteOffset(actor) + base;
    r1v = IvyInit_GetPaletteBase(actor) + IvyInit_GetPaletteOffset(actor);
    if (IvyInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (IvyInit_GetDrawableKind(actor) == 1)
        IvyInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20), SceneActorScript_GetManagerField62());
}

static inline void IvyInit_InstallPrimary(struct IvyInitActor *arg0,
    void *cb0,
    u32 (*cb1)(void *),
    void *cb2,
    void *cb3,
    void *cb4, void *hidden,
    void *cb5,
    void *cb6, void *defaultReaction, void *cb270,
    void *targetQuery, void *damage, void *contact)
{
    IvyInit_SetBehaviorFlags(arg0, 0x2000);
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
    IvyInit_ClearMotion(arg0);
}

static inline void IvyInit_InitializeGroup(struct IvyInitGroup *group, void *firstBounds,
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

static inline void IvyInit_InitializeRegion(struct IvyInitActor *arg0)
{
    if (IvyInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);

}

static inline void IvyInit_InstallCompletionCallbacks(struct IvyInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Ivy_UpdateInteraction;
    arg0->field25C = Ivy_Destroy;

}

static inline u8 *IvyInit_Drawable(struct IvyInitActor *actor)
{
    return actor->drawable;
}

static inline void IvyInit_SetCategory(void *object, u8 category)
{
    struct IvyInitActor *actor = object;
    actor->category = category;
}

static inline void IvyInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void IvyInit_InstallRoutes(struct IvyInitActor *actor)
{
    actor->field248 = Ivy_TickDrawable;
    actor->field250 = gUnk_08606400;
}
static inline void IvyInit_ClearReceiver(struct IvyInitActor *actor)
{
    actor->field304 = 0;
}

static inline void IvyInit_InitializeWidget(u8 *drawable, u16 script, u16 resource, struct IvyInitPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct IvyInit32Block *)drawable = *(struct IvyInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline void IvyInit_ResetCapture(void *object)
{
    struct IvyInitActor *actor = object;
    actor->captureFlags = 0;
}

static inline u8 IvyInit_TestGameFlags(u32 mask)
{
    if (*(u32 *)(gGameState + 0x868) & mask)
        return 1;
    return 0;
}

static inline void IvyInit_EnableColliderContact(struct IvyInitCollider *collider)
{
    u32 flags = collider->flags;
    u32 mask = 8;
    mask |= flags;
    mask &= ~4;
    collider->flags = mask;
}

static inline void IvyInit_ClearMotionFlags(struct IvyInitActor *actor, u32 mask)
{
    actor->field2C0 = mask & actor->field2C0;
}

s32 Ivy_Init(void *object)
{
    struct IvyInitActor *arg0 = object;
    struct IvyInitActor *actor = object;
    struct IvyInitGroup *group = &arg0->group;
    u32 zero;
    struct IvyInitPosition tmp;
    u32 flag;

    IvyInit_ResetCapture(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    IvyInit_SetCategory(actor, 24);
    Ivy_InitProfileId(actor);
    if (IvyInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = IvyInit_Drawable(actor);
        IvyInit_InitializeSprite(heap, 0xD638, 0x99DD);
        if (IvyInit_GetCategory(actor) == 0x11 || IvyInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = IvyInit_Drawable(actor);
        IvyInit_ClearPosition(&zero, &tmp);
        IvyInit_InitializeWidget(heap, 0xCB05, 0xD638, &tmp);
        actor->field2B7 = 0xFF;
    }
    IvyInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        arg0->field636 = IvyInit_ReadHalfword(0);
        arg0->field63E = IvyInit_ReadByte(0);
        arg0->field63D = IvyInit_ReadByte(0);
    }
    arg0->field638 = Script_GetKeywordValue('d', 0);
    actor->body.position.x += 0x20;
    actor->body.position.z += 0x20;
    IvyInit_DrawablePosition(actor);
    IvyInit_ReadPalette(actor, 0x12A, 0x192);
    SceneActor_InitCollision5005(actor);
    IvyInit_InstallPrimary(actor, Ivy_Update, 0, 0, 0, 0, 0,
        0, Ivy_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0, 0,
        sub_081363F4, SceneActorControl_BeginSoundTransition);
    IvyInit_InitializeGroup(group, 0, 0, 0, 0, 0, 0);
    IvyInit_InstallRoutes(actor);
    IvyInit_SetBehaviorFlags(actor, 0x200);
    IvyInit_SetStatusFlags(actor, 0x408);
    IvyInit_SetMotionFlags(actor, 0xC0);
    if (Script_GetKeywordValue('E', 0))
        IvyInit_SetStatusFlags(actor, 0x2000000);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (IvyInit_TestMotionFlags(actor, 4)) {
        IvyInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        IvyInit_ResetState(actor, 0);
        IvyInit_SetControl(actor, IvyInit_GetStandingControl(actor), 0);
    }
    IvyInit_InitializeRegion(actor);
    if (IvyInit_TestMotionFlags(actor, 0x40))
        IvyInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !IvyInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (IvyInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    IvyInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Ivy_InitControlPresets(actor);
    SceneActor_BeginReaction22(actor);
    if (IvyInit_TestGameFlags(0x400)) {
        IvyInit_ResetState(actor, 1);
        IvyInit_SetControl(actor, SceneActorReaction_PlayHiddenCue, 0x2A);
        IvyInit_EnableColliderContact((void *)((u8 *)actor + 0x134));
        IvyInit_ClearMotionFlags(actor, ~0x800);
    }
    IvyInit_SaveControl(IvyInit_SavedControlOf(actor), actor);
    IvyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
