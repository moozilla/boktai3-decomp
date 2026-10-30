#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "player.h"

void Axe_InitProfile(void *);
void Axe_InitAttackCooldowns(void *);
void ActorGroup_ReadScriptOptions(void *);
void Axe_InitEffectPalettes(void *);
void Axe_InitColliders(void *);
void Axe_OnUpdate(void);
s32 Axe_GetActorCategory(void);
void Axe_UpdateTurnAnimation(void);
void Axe_OnAnimationReset(void);
void sub_080F93A4(void);
void Axe_ConvergeRecoveryMotion(void);
void Axe_UpdateDefeat(void);
s32 SceneActorControl_CanAdvanceHover(void *);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorControl_AdvanceReactionCount(void);
void SceneActorControl_InitializeHoverState(void);
void Axe_OnReaction(void);
void Axe_ReleaseAndIdle(void);
void Axe_UpdateDamageSpin(void);
void SceneActorBehavior_InitializeShadow(void *);
void Axe_InitMotionTable(void *);
void Axe_InitActionCallbacks(void *);
void Axe_InitEventCallbacks(void *);
void SpriteGroup_InitAnimatedSprites(void *);
void Axe_InitRoutePointHistory(void *);
void ActorCollision_InitThreeAuxiliaryAttacks(void *);
void Axe_OnRender(void);
void Axe_ReleaseResources(void);

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

s32 Script_GetValue(void);
u32 Script_GetPc(void);
static inline s32 AxeInit_Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
#define SV(d) AxeInit_Value(d)

struct AxeInit32Block {
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

struct AxeInitPosition {
    s16 x, y, z, pad;
};
struct AxeInitMap {
    u32 header;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
static inline void AxeInit_ClearPosition(u32 *fill, void *object)
{
    struct AxeInitPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}
static inline u16 AxeInit_PositionX(const u16 *pos)
{
    return pos[0];
}
static inline u16 AxeInit_PositionZ(const u16 *pos)
{
    return pos[2];
}
static inline s32 AxeInit_Cell(u16 *pos)
{
    s32 x = (s16)AxeInit_PositionX(pos) >> 8;
    s32 z = (s16)AxeInit_PositionZ(pos) >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct AxeInitMap *)gTerrainMap)->rows[z] + x;
}
static inline u32 AxeInit_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}

static inline u32 AxeInit_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}

static inline u32 AxeInit_Height(s32 cell, u16 *pos)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    u32 attributes, shape;
    s32 height;
    if (p)
        p += 4;
    else {
        struct AxeInitMap *map = (void *)gTerrainMap;
        u32 offset = cell * 4 + 12;
        p = map->cells + offset;
    }
    shape = AxeInit_TerrainShape(p);
    height = AxeInit_TerrainBaseHeight(p);
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

struct AxeInitBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct AxeInitPosition position;
    u8 unknown64[0x34];
};
struct AxeInitAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct AxeInitGate {
    u16 field414;
    u16 field416;
    u16 field418;
    u16 field41A;
    void *field41C;
};
struct AxeInitRange {
    u32 field420 : 16;
    u32 field422 : 16;
    u32 field424;
    u8 unknown428[0x2];
    u8 field42A;
    u8 field42B;
};
struct AxeInitRoutePoint {
    s16 x, z;
};
struct AxeInitRoute {
    u32 unknown0, unknown4;
    struct AxeInitRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameterB, pointId, unknown13;
};
struct AxeInitGroup {
    u16 field3D4;
    u8 unknown3D6[0x2];
    u32 field3D8;
    u8 unknown3DC[0x38];
    struct AxeInitGate gate;
    struct AxeInitRange range;
    u8 unknown42C[0x10];
    void *field43C;
    u32 field440;
    void *field444;
    struct AxeInitRoute route;
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
struct AxeInitTargeting {
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
struct AxeInitSavedControl {
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
struct AxeInitActor {
    u8 unknown0[0x4];
    u8 *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct AxeInitBody body;
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
    struct AxeInitAttacks attacks;
    struct AxeInitTargeting targeting;
    u8 unknown100[0x4];
    u32 field104;
    u32 field108;
    u32 field10C;
    u32 field110;
    u32 field114;
    struct AxeInitSavedControl saved;
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
    void *field220;
    void *field224;
    void *field228;
    void *field22C;
    void *field230;
    void *field234;
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
    s32 (*field260)(void);
    void *field264;
    void *field268;
    void *field26C;
    s32 (*field270)(void *);
    void *field274;
    void *field278;
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
    struct AxeInitGroup group;
    u8 unknown624[0x824];
    s32 halfHealthE48;
    u32 savedFlagsE4C;
    u8 unknownE50[8];
    u16 groundHeightE58;
    u8 unknownE5A[0x5C];
    u16 fieldEB6;
    u8 unknownEB8[0x1F];
    u8 variantED7;
    u8 unknownED8[0x44];
    struct SceneVec playerPositionF1C;
};

static inline u8 AxeInit_GetDrawableKind(struct AxeInitActor *actor)
{
    return actor->drawableKind;
}
static inline u8 AxeInit_GetCategory(void *object)
{
    struct AxeInitActor *actor = object;
    return actor->category;
}
static inline s32 AxeInit_GetHealth(struct AxeInitActor *actor)
{
    return actor->field9C;
}
static inline u16 AxeInit_GetInitialHealth(struct AxeInitActor *actor)
{
    return actor->fieldB0;
}

static inline u32 AxeInit_GetPaletteBase(struct AxeInitActor *actor)
{
    return actor->field1E0;
}

static inline u32 AxeInit_GetPaletteOffset(struct AxeInitActor *actor)
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

static inline u16 AxeInit_GetMotionFlags(struct AxeInitActor *actor)
{
    return actor->field2C0;
}
static inline u16 AxeInit_GetHeightOffset(struct AxeInitActor *actor)
{
    return actor->field2EA;
}
static inline u32 AxeInit_GetBehaviorFlags(struct AxeInitActor *actor)
{
    return actor->field2CC;
}
static inline u32 AxeInit_GetStatusFlags(struct AxeInitActor *actor)
{
    return actor->field2D0;
}
static inline u8 AxeInit_GetState(struct AxeInitActor *actor)
{
    return actor->field2A8;
}
static inline u8 AxeInit_GetPhase(struct AxeInitActor *actor)
{
    return actor->field2A9;
}
static inline u8 AxeInit_GetBehavior(struct AxeInitActor *actor)
{
    return actor->field2AA;
}
static inline void *AxeInit_GetCurrentControl(struct AxeInitActor *actor)
{
    return actor->field21C;
}
static inline u8 AxeInit_GetControlKind(struct AxeInitActor *actor)
{
    return actor->field2B5;
}
static inline u8 AxeInit_GetDirection(struct AxeInitActor *actor)
{
    return actor->body.direction;
}
static inline u32 AxeInit_GetTerrainCell(struct AxeInitActor *actor)
{
    return actor->field2F4;
}
static inline u32 AxeInit_GetTargetingFlags(struct AxeInitTargeting *target)
{
    return target->fieldEB;
}
static inline struct AxeInitSavedControl *AxeInit_SavedControlOf(struct AxeInitActor *actor)
{
    struct AxeInitSavedControl *saved = &actor->saved;
    return saved;
}

static inline void AxeInit_SaveControl(struct AxeInitSavedControl *saved,
                                       struct AxeInitActor *actor)
{
    saved->field12E = AxeInit_GetMotionFlags(actor);
    saved->field12C = AxeInit_GetHeightOffset(actor);
    saved->field118 = AxeInit_GetBehaviorFlags(actor);
    saved->field11C = AxeInit_GetStatusFlags(actor);
    saved->field120 = AxeInit_GetDirection(actor);
    saved->field121 = AxeInit_GetState(actor);
    saved->field122 = AxeInit_GetPhase(actor);
    saved->field123 = AxeInit_GetBehavior(actor);
    saved->field124 = AxeInit_GetCurrentControl(actor);
    saved->field128 = AxeInit_GetControlKind(actor);
}
struct AxeInitCollider {
    u8 header[6];
    u16 flags;
};
static inline void AxeInit_ClearColliderFlags(struct AxeInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
static inline s32 AxeInit_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline void AxeInit_SetBehaviorFlags(struct AxeInitActor *actor, u32 mask)
{
    actor->field2CC |= mask;
}

static inline void AxeInit_SetStatusFlags(struct AxeInitActor *actor, u32 mask)
{
    actor->field2D0 |= mask;
}

static inline void AxeInit_SetMotionFlags(struct AxeInitActor *actor, u16 mask)
{
    actor->field2C0 |= mask;
}
static inline u8 AxeInit_TestMotionFlags(struct AxeInitActor *actor, u32 mask)
{
    if (AxeInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}

static inline u8 AxeInit_TestTargetingFlags(struct AxeInitTargeting *target, u32 mask)
{
    if (AxeInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}

static inline void AxeInit_ClearControlFlags(struct AxeInitActor *actor, u32 mask)
{
    actor->field114 &= mask;
}
static inline u8 AxeInit_IsInCurrentRegion(struct AxeInitActor *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->field2FA)
        return 1;
    return 0;
}
static inline void *AxeInit_GetStandingControl(struct AxeInitActor *actor)
{
    return actor->field22C;
}

static inline void AxeInit_SetControl(struct AxeInitActor *actor, void *callback, u32 behavior)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = behavior;
    actor->field21C = callback;
    actor->field2C8 = 0;
    AxeInit_ClearControlFlags(actor, ~1);
    actor->field104 = 0;
}
static inline void AxeInit_SetState(struct AxeInitActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->field2A8 = state;
    actor->field2A9 = phase;
    actor->field2AA = behavior;
    actor->field2AC = 0;
    actor->field2B1 = 1;
}

static inline void AxeInit_InitializeGate(struct AxeInitGate *gate, u32 parameter)
{
    gate->field414 = 0;
    gate->field416 = parameter;
    gate->field418 = 0;
    gate->field41A = 0;
    gate->field41C = 0;
}

static inline void AxeInit_ReadTargeting(struct AxeInitTargeting *target)
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
    if (AxeInit_TestTargetingFlags(target, 4))
        target->fieldF4 = target->fieldF8;
    else
        target->fieldF4 = Div(target->fieldF8 * 8 - target->fieldF8, 10);
}

static inline u16 AxeInit_RangeRadius(struct AxeInitRange *range)
{
    return range->field422;
}
static inline void AxeInit_ReadRange(struct AxeInitRange *range)
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
    range->field424 = AxeInit_RangeRadius(range) * AxeInit_RangeRadius(range);
    range->field42A = 0;
}

static inline u32 AxeInit_RoutePointX(struct AxeInitRoute *route)
{
    return (u16)route->point->x;
}

static inline u32 AxeInit_RoutePointZ(struct AxeInitRoute *route)
{
    return (u16)route->point->z;
}

static inline void AxeInit_SnapToRoute(struct AxeInitPosition *position, struct AxeInitRoute *route)
{
    position->x = AxeInit_RoutePointX(route);
    position->z = AxeInit_RoutePointZ(route);
    position->y = AxeInit_Height(AxeInit_Cell(position), (u16 *)position);
}
static inline void AxeInit_DrawablePosition(struct AxeInitActor *actor)
{
    if (AxeInit_GetDrawableKind(actor) == 0)
        *(struct AxeInitPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct AxeInitPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline u8 AxeInit_RouteId(struct AxeInitRoute *route)
{
    return route->routeId;
}
static inline u8 AxeInit_RouteParameter(struct AxeInitRoute *route)
{
    return route->parameterB;
}
static inline u8 AxeInit_RoutePointId(struct AxeInitRoute *route)
{
    return route->pointId;
}
static inline u8 AxeInit_RouteActiveBranch(struct AxeInitRoute *route)
{
    return route->activeBranch;
}
static inline u16 AxeInit_ReadKeywordHalfword(u32 keyword, s32 fallback)
{
    return Script_GetKeywordValue(keyword, fallback);
}

static inline s32 AxeInit_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}

static inline void AxeInit_InstallIdle(struct AxeInitActor *actor, void *idle)
{
    actor->field21C = idle;
}

static inline void AxeInit_InitializeHealth(struct AxeInitActor *arg0, u16 initialHealth,
                                            s32 firstLimit, s32 secondLimit)
{
    arg0->field2D4 = 0;
    AxeInit_InstallIdle(arg0, SceneActorControl_OnIdle);
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->field9C = AxeInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (secondLimit <= 0)
        secondLimit = firstLimit;
    arg0->fieldA6 = firstLimit;
    arg0->fieldA8 = secondLimit;
    arg0->field2FA = 0;
}

static inline void AxeInit_SetInitialHealth(struct AxeInitActor *actor, u16 health)
{
    actor->fieldB0 = health;
}

static inline u32 AxeInit_RouteAlternateBranch(struct AxeInitRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline void AxeInit_BeginRoute(struct AxeInitBody *body, struct AxeInitRoute *route)
{
    SceneBody_StartRoute(body, route, AxeInit_RouteId(route), AxeInit_RouteParameter(route),
                         AxeInit_RoutePointId(route));
}

static inline u8 AxeInit_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}

static inline u16 AxeInit_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline void AxeInit_SetFirstAttack(struct AxeInitActor *actor, s32 index, s32 value)
{
    actor->attacks.first[index] = value;
}

static inline u32 AxeInit_ReadRoute(struct AxeInitActor *arg0, struct AxeInitGroup *group,
                                    struct AxeInitRoute *rt)
{
    struct AxeInitPosition point;
    s32 r1v;
    rt->activeBranch = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt->routeId = SV(0);
        rt->parameterB = 0;
        r1v = SV(0);
        rt->pointId = r1v;
        rt->currentPoint = r1v;
        rt->branches[AxeInit_RouteActiveBranch(rt)] = rt->routeId;
        rt->branches[AxeInit_RouteAlternateBranch(rt)] =
            Script_GetPc() ? Script_GetValue() : AxeInit_RouteId(rt);
        Path_GetPointPositionById((u16 *)&point, AxeInit_RouteId(rt), AxeInit_RoutePointId(rt));
        arg0->field2F4 = AxeInit_Cell(&point);
        point.y = arg0->field2BE = AxeInit_Height(AxeInit_GetTerrainCell(arg0), (u16 *)&point);
        AxeInit_BeginRoute(&arg0->body, rt);
        return 1;
    } else {
        rt->routeId = 0;
        rt->parameterB = 0;
        rt->pointId = 0;
        rt->currentPoint = 0;
        group->route.branches[AxeInit_RouteActiveBranch(rt)] = 0;
        group->route.branches[AxeInit_RouteAlternateBranch(rt)] = 0;
    }

    return 0;
}

static inline void AxeInit_InitializeBody(void *object, u32 *fill)
{
    struct AxeInitActor *arg0 = object;
    struct AxeInitGroup *group = &arg0->group;
    struct AxeInitBody *body = &arg0->body;
    struct AxeInitPosition origin;
    struct AxeInitPosition point;
    s32 mode, i, r4v, r6v, r5v, r2v, r1v;
    u16 initialHealth;
    struct AxeInitRoute *rt;
    arg0->field2CC = 0;
    arg0->field2D0 = 0;
    arg0->field2B4 = 0;
    arg0->field2E8 = 8;
    arg0->field2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    AxeInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        i = 0;
        while (1) {
            AxeInit_SetFirstAttack(arg0, i, AxeInit_ReadHalfword(0x18));
            arg0->attacks.second[i] = AxeInit_ReadHalfword(0x18);
            arg0->attacks.flags[i] = SV(0);
            arg0->attacks.parameter[i] = AxeInit_ReadHalfword(0);
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
    AxeInit_InitializeGate(&group->gate, r2v);
    AxeInit_ReadRange(&group->range);
    AxeInit_ReadTargeting(&arg0->targeting);
    if (Script_GetKeywordValue(0x62, 0))
        AxeInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = AxeInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = AxeInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        AxeInit_SetInitialHealth(arg0, AxeInit_ReadHalfword(0));
        group->field5B6 = AxeInit_Half(AxeInit_GetInitialHealth(arg0));
        arg0->field9E = AxeInit_ReadByte(0);
        arg0->field9F = AxeInit_ReadByte(0);
        arg0->fieldA0 = AxeInit_ReadByte(0);
        arg0->fieldA2 = AxeInit_ReadHalfword(0);
        arg0->fieldA4 = AxeInit_ReadHalfword(0);
    }

    AxeInit_InitializeHealth(arg0, AxeInit_GetInitialHealth(arg0), r5v, r6v);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = SV(0);
        arg0->fieldA1 = AxeInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    r1v = (s16)Script_GetKeywordValue(0x56, -1);
    if (r1v < 0)
        r1v = 0xFFFF;
    arg0->field9A = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        AxeInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        AxeInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->route;
    if (AxeInit_ReadRoute(arg0, group, rt)) {
        AxeInit_SnapToRoute(&arg0->body.position, rt);
        AxeInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (AxeInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, AxeInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (AxeInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->field3D4 = arg0->body.id;
    group->field3D8 = (u32)&arg0->body.position;
}

struct AxeInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void AxeInit_SetWidgetScriptPalette(struct AxeInitWidget *widget, s32 scriptPalette)
{
    widget->paletteBank = scriptPalette - 0x5FFF;
}
static inline void AxeInit_SetMotionControl(struct AxeInitActor *actor, void *callback, u32 kind)
{
    actor->field104 = (u32)callback;
    actor->field108 = 0;
    actor->field10C = 0;
    actor->field110 = kind;
    actor->field114 = 1;
}
static inline void AxeInit_ClearMotion(void *object)
{
    struct AxeInitActor *actor = object;
    u32 zero = 0;
    AxeInit_SetMotionControl(actor, 0, 0);
    actor->field114 = zero;
}
static inline void AxeInit_ReadPalette(struct AxeInitActor *actor, u32 base, u32 alternate)
{
    s32 r1v;
    actor->field1E6 = Script_GetKeywordValue(0x43, 0);
    actor->field1E0 = base;
    actor->field1E2 = alternate;
    actor->field1E4 = AxeInit_GetPaletteOffset(actor) + base;
    r1v = AxeInit_GetPaletteBase(actor) + AxeInit_GetPaletteOffset(actor);
    if (AxeInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (AxeInit_GetDrawableKind(actor) == 1)
        AxeInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                       SceneActorScript_GetManagerField62());
}

static inline void AxeInit_InstallPrimary(struct AxeInitActor *arg0, void *cb0, s32 (*cb1)(void),
                                          void *cb2, void *cb3, void *cb4, void *hidden, void *cb5,
                                          void *cb6, void *defaultReaction, s32 (*cb270)(void *),
                                          void *targetQuery, void *damage, void *contact)
{
    AxeInit_SetBehaviorFlags(arg0, 0x2000);
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
    AxeInit_ClearMotion(arg0);
}

static inline void AxeInit_InitializeGroup(struct AxeInitGroup *group, void *firstBounds,
                                           void *secondBounds, void *downed, void *returnToRoute,
                                           void *recover, void *animate)
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

static inline void AxeInit_InitializeRegion(struct AxeInitActor *arg0)
{
    if (AxeInit_TestMotionFlags(arg0, 2))
        arg0->field2FA = *(u32 *)(gGameState + 0x5A0);
    else
        arg0->field2FA = (u16)(u32)Region_FindContaining(&arg0->body.position);
}

static inline void AxeInit_InstallCompletionCallbacks(struct AxeInitActor *arg0)
{
    arg0->field24C = SceneActorScript_OnCommand;
    arg0->field258 = Axe_OnRender;
    arg0->field25C = Axe_ReleaseResources;
}

static inline u8 *AxeInit_Drawable(struct AxeInitActor *actor)
{
    return actor->drawable;
}

static inline void AxeInit_SetCategory(struct AxeInitActor *actor, u8 category)
{
    actor->category = category;
}

static inline void AxeInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void AxeInit_InstallRoutes(struct AxeInitActor *actor)
{
    actor->field248 = SceneActorControl_InitializeHoverState;
    actor->field23C = Axe_OnReaction;
}
static inline void AxeInit_ClearReceiver(struct AxeInitActor *actor)
{
    actor->field304 = 0;
}

static inline void AxeInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                            struct AxeInitPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct AxeInit32Block *)drawable = *(struct AxeInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}

static inline u8 AxeInit_BoundedVariant(s32 variant)
{
    if (variant > 1)
        return 1;
    return variant;
}

static inline void AxeInit_SetVariant(struct AxeInitActor *actor, u8 variant)
{
    actor->variantED7 = variant;
}

static inline void AxeInit_InitializePaletteRange(struct AxeInitActor *actor, u32 alternate,
                                                  u32 current)
{
    actor->field1DC = AxeInit_GetPaletteBase(actor) + alternate;
    actor->field1DE = AxeInit_GetPaletteBase(actor) + current;
}

static inline void AxeInit_SetCurrentPalette(struct AxeInitActor *actor, u16 palette)
{
    actor->field1E4 = palette;
}

s32 Axe_Init(void *object)
{
    struct AxeInitActor *arg0 = object;
    struct AxeInitActor *actor = object;
    struct AxeInitGroup *group = &arg0->group;
    u32 zero;
    struct AxeInitPosition tmp;
    u32 flag;
    s32 variant;

    arg0->savedFlagsE4C = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    actor->variantED7 = Script_GetKeywordValue('t', 0);
    variant = AxeInit_BoundedVariant(actor->variantED7);
    AxeInit_SetVariant(arg0, variant);
    if (variant == 0)
        AxeInit_SetCategory(actor, 12);
    else
        AxeInit_SetCategory(actor, 13);
    Axe_InitProfile(actor);
    if (AxeInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = AxeInit_Drawable(actor);
        AxeInit_InitializeSprite(heap, 0x5291, 0x3F72);
        if (AxeInit_GetCategory(actor) == 0x11 || AxeInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = AxeInit_Drawable(actor);
        AxeInit_ClearPosition(&zero, &tmp);
        AxeInit_InitializeWidget(heap, 0xCB05, 0x5291, &tmp);
        actor->field2B7 = 0xFF;
    }
    AxeInit_InitializeBody(arg0, &zero);
    arg0->halfHealthE48 = AxeInit_GetHealth(actor) >> 1;
    Axe_InitAttackCooldowns(actor);
    if (Script_SeekToKeyword('p')) {
        u32 height;
        actor->body.position.x = AxeInit_ReadHalfword(0);
        actor->body.position.y = AxeInit_ReadHalfword(0);
        actor->body.position.z = AxeInit_ReadHalfword(0);
        actor->field2F4 = AxeInit_Cell((u16 *)&actor->body.position);
        height = AxeInit_Height(AxeInit_GetTerrainCell(actor), (u16 *)&actor->body.position);
        actor->field2BE = height;
        arg0->groundHeightE58 = height;
    } else {
        arg0->groundHeightE58 = actor->field2BE;
    }
    arg0->fieldEB6 = Script_GetKeywordValue('w', -1);
    ActorGroup_ReadScriptOptions(actor);
    arg0->playerPositionF1C = gPlayers[0]->position;
    AxeInit_ReadPalette(actor, 0x2BD, 0x192);
    AxeInit_InitializePaletteRange(actor, 0, 2);
    Axe_InitEffectPalettes(actor);
    AxeInit_SetCurrentPalette(actor, 0x1EE);
    Axe_InitColliders(actor);
    AxeInit_InstallPrimary(actor, Axe_OnUpdate, Axe_GetActorCategory, Axe_UpdateTurnAnimation,
                           Axe_OnAnimationReset, sub_080F93A4, 0, Axe_ConvergeRecoveryMotion,
                           Axe_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
                           SceneActorControl_CanAdvanceHover, SceneActorTarget_ResolvePendingObject,
                           sub_081363F4, SceneActorControl_AdvanceReactionCount);
    AxeInit_InitializeGroup(group, (u8 *)arg0 + 0xED8, (u8 *)arg0 + 0xEFC, 0, 0, 0, 0);
    AxeInit_InstallRoutes(actor);
    group->field61C = 0;
    group->field5EC = 0;
    group->field614 = Axe_ReleaseAndIdle;
    group->field610 = Axe_UpdateDamageSpin;
    AxeInit_SetMotionFlags(actor, 0x60);
    AxeInit_SetBehaviorFlags(actor, 0x200);
    AxeInit_SetStatusFlags(actor, 0x02100408);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (AxeInit_TestMotionFlags(actor, 4)) {
        AxeInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        AxeInit_SetState(actor, 0, 0, 0);
        AxeInit_SetControl(actor, AxeInit_GetStandingControl(actor), 0);
    }
    AxeInit_InitializeRegion(actor);
    if (AxeInit_TestMotionFlags(actor, 0x40))
        AxeInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !AxeInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (AxeInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    AxeInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Axe_InitMotionTable(actor);
    Axe_InitActionCallbacks(actor);
    Axe_InitEventCallbacks(actor);
    actor->drawable[7] = 1;
    actor->field2E8 = 0;
    SpriteGroup_InitAnimatedSprites(actor);
    Axe_InitRoutePointHistory(actor);
    ActorCollision_InitThreeAuxiliaryAttacks(actor);
    AxeInit_SaveControl(AxeInit_SavedControlOf(actor), actor);
    actor->saved.field130 = arg0->savedFlagsE4C;
    AxeInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
