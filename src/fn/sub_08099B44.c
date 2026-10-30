#include "global.h"
#include "scene_actor_control_motion.h"
#include "display_sequence_inline.h"
#include "scene_actor_terrain.h"
#include "render.h"
#include "player.h"
#include "game_state.h"
#include "scene_actor_animation.h"
#define U8(p, n)  (*((u8 *)(p) + (n)))
#define U32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define PTR(p, n) (*(void **)((u8 *)(p) + (n)))
#define POS(p, n) (*(struct ContactPosition *)((u8 *)(p) + (n)))
void CpuSet(const void *, void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, u32, void *);
void Ghost_InitActorProfile(void *);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);
void SceneBody_Init(void *, u32, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, u32, u32);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Route_GetCurrentPointFacing(void *);
s32 SceneActorScript_GetManagerField62(void);
void SceneActor_InitLinkedReactionColliders(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 Region_FindContaining(void *);
void SceneActorPlacement_HideActor(void *, u32);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void Ghost_InitMotionPreset(void *);
void Ghost_InitActionTable(void *);
void Ghost_InitReactionTable(void *);
void Ghost_InitEffectTable(void *);
void Ghost_GetDirectionalFrame(u8, u32 *, u32 *);
void SceneActor_InitLinkedSprites(void *);
u8 SceneActorRegistry_LinkActor(void *);
void SceneActorControl_OnIdle(void);
void Ghost_OnBehaviorUpdate(void);
u32 Ghost_GetBehaviorCode(void *);
void Ghost_TurnTowardHeading(void);
void Ghost_UpdateDirectionalPose(void);
void Ghost_UpdateDirectionalPoseB(void);
void Ghost_UpdateHitStagger(void);
void Ghost_UpdateDefeat(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void Ghost_ResetMovementState(void);
void Ghost_OnControlCleanup(void);
u8 Ghost_GetControlStatus(void *);
void SceneActorScript_OnCommand(void);
void Ghost_OnActorUpdate(void);
void Ghost_Destroy(void);
extern const u8 gUnk_08605E14[];
struct GhostRoutePoint {
    s16 x, z;
};
struct GhostBody {
    u32 id;
    u8 unknown54[0x5];
    u8 direction;
    u8 unknown5A[0x2];
    struct ContactPosition position;
    u8 unknown64[0x34];
};
struct GhostAttacks {
    u16 first[4];
    u16 second[4];
    u16 parameter[4];
    u32 flags[4];
};
struct GhostRoute {
    u32 unknown0, unknown4;
    struct GhostRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameter, pointId, unknown13;
};
struct GhostTargeting {
    u32 target;
    u8 unknown4[10];
    u8 heading, flags;
    u16 x, z;
    u32 distance, range, limit, zSquared;
};
struct GhostSavedControl {
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
struct GhostActor {
    u8 unknown0[0x4];
    u8 *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct GhostBody body;
    u8 category;
    u8 unknown99[0x1];
    u16 field9A;
    u16 health;
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
    u16 defaultHealth;
    u16 fieldB2;
    struct GhostAttacks attacks;
    struct GhostTargeting targeting;
    u8 unknown100[0x4];
    u32 word104;
    u32 word108;
    u32 word10C;
    u32 word110;
    u32 flags114;
    struct GhostSavedControl saved;
    struct ContactCollider colliderA, colliderB;
    u16 alternatePalette, paletteBank;
    u16 basePaletteBank;
    u16 paletteAlternateBank;
    u16 alternateOffset;
    u16 paletteOffset;
    u16 field1E8, paletteFlashBank;
    u32 field1EC;
    u32 field1F0;
    u8 unknown1F4[0x28];
    void *target;
    void *field220;
    void *entryTarget224;
    void *entryTarget228;
    void *savedTarget;
    void *reactionTarget230;
    void *field234;
    void *field238;
    void *field23C;
    u32 field240;
    u8 unknown244[0x4];
    void *callback248;
    void *field24C;
    const void *field250;
    u8 unknown254[0x4];
    void *field258;
    void *field25C;
    u32 (*field260)(void *);
    void *field264;
    void *field268;
    void *update26C;
    void *update270;
    void *field274;
    void *field278;
    u8 unknown27C[0x2C];
    u8 state2A8;
    u8 phase2A9;
    u8 behavior2AA;
    u8 unknown2AB[0x1];
    u32 counter2AC;
    u8 field2B0;
    u8 field2B1;
    u8 field2B2;
    u8 field2B3;
    u8 enabled;
    u8 field2B5;
    u8 unknown2B6[0x1];
    u8 field2B7;
    u8 unknown2B8[0x6];
    u16 groundHeight2BE;
    u16 flags2C0;
    u16 field2C2;
    u32 profileMode;
    u16 field2C8;
    u8 unknown2CA[0x2];
    u32 flags2CC;
    u32 flags2D0;
    u16 flags2D4;
    u8 unknown2D6[0x12];
    u16 field2E8;
    u16 field2EA;
    u8 unknown2EC[0x8];
    u16 terrainCell;
    u8 unknown2F6[0x4];
    u16 field2FA;
    u8 unknown2FC[0x8];
    u32 field304;
    u32 profileId;
    u8 unknown30C[0xC4];
    struct GhostActor *context;
    u8 unknown3D4[0x184];
    u32 flags558;
    u8 unknown55C[4];
    u8 phase560, phase561;
    u8 unknown562[2];
    u16 baseHeight;
    u8 unknown566[0x4C];
    u16 visibilityTimer;
    u8 unknown5B4[8];
    u8 phase5BC;
    u8 unknown5BD[3];
    u16 effectDuration;
    u8 unknown5C2[2];
    u32 distanceSquared;
    u8 unknown5C8[0x3C];
    struct SceneVec playerPosition;
    struct GhostRoute route;
    u8 unknown620[2];
    u16 routeDelay;
};
struct ResourceCopy {
    u32 words[8];
};
static inline u8 Ghost_GetDrawableKind(struct GhostActor *a)
{
    return a->drawableKind;
}
static inline u16 Ghost_GetInitialHealth(struct GhostActor *a)
{
    return a->defaultHealth;
}
static inline u16 Ghost_GetMotionFlags(struct GhostActor *a)
{
    return a->flags2C0;
}
static inline u16 Ghost_GetHeightOffset(struct GhostActor *a)
{
    return a->field2EA;
}
static inline u32 Ghost_GetBehaviorFlags(struct GhostActor *a)
{
    return a->flags2CC;
}
static inline u32 Ghost_GetStatusFlags(struct GhostActor *a)
{
    return a->flags2D0;
}
static inline u8 Ghost_GetDirection(struct GhostActor *a)
{
    return a->body.direction;
}
static inline u8 Ghost_GetState(struct GhostActor *a)
{
    return a->state2A8;
}
static inline u8 Ghost_GetPhase(struct GhostActor *a)
{
    return a->phase2A9;
}
static inline u8 Ghost_GetBehavior(struct GhostActor *a)
{
    return a->behavior2AA;
}
static inline void *Ghost_GetCurrentControl(struct GhostActor *a)
{
    return a->target;
}
static inline u8 Ghost_GetControlKind(struct GhostActor *a)
{
    return a->field2B5;
}
static inline u16 Ghost_GetPaletteBase(struct GhostActor *a)
{
    return a->basePaletteBank;
}
static inline u16 Ghost_GetPaletteOffset(struct GhostActor *a)
{
    return a->paletteOffset;
}
static inline u32 Ghost_GetTerrainCell(struct GhostActor *a)
{
    return a->terrainCell;
}
static inline void Ghost_SetInstanceFlags(struct GhostActor *a, u32 mask)
{
    a->flags558 |= mask;
}
static inline u8 Ghost_TestInstanceFlags(struct GhostActor *a, u32 mask)
{
    if (a->flags558 & mask)
        return 1;
    return 0;
}
static inline u8 Ghost_TestWorldFlags(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
static inline struct GhostSavedControl *Ghost_SavedControlOf(struct GhostActor *a)
{
    return &a->saved;
}
static inline void Ghost_SetMotionFlags(struct GhostActor *a, u16 mask)
{
    a->flags2C0 |= mask;
}
static inline u8 Ghost_TestMotionFlags(struct GhostActor *a, u32 mask)
{
    if (Ghost_GetMotionFlags(a) & mask)
        return 1;
    return 0;
}
static inline void Ghost_SetBehaviorFlags(struct GhostActor *a, u32 mask)
{
    a->flags2CC |= mask;
}
static inline void Ghost_SetStatusFlags(struct GhostActor *a, u32 mask)
{
    a->flags2D0 |= mask;
}
static inline u8 Ghost_GetTargetingFlags(struct GhostTargeting *target)
{
    return target->flags;
}
static inline void Ghost_SaveControl(struct GhostSavedControl *saved, struct GhostActor *a)
{
    saved->field12E = Ghost_GetMotionFlags(a);
    saved->field12C = Ghost_GetHeightOffset(a);
    saved->field118 = Ghost_GetBehaviorFlags(a);
    saved->field11C = Ghost_GetStatusFlags(a);
    saved->field120 = Ghost_GetDirection(a);
    saved->field121 = Ghost_GetState(a);
    saved->field122 = Ghost_GetPhase(a);
    saved->field123 = Ghost_GetBehavior(a);
    saved->field124 = Ghost_GetCurrentControl(a);
    saved->field128 = Ghost_GetControlKind(a);
}
static inline void Ghost_ClearPosition(u32 *fill, void *object)
{
    struct ContactPosition *position = object;
    *fill = 0;
    CpuSet(fill, position, 0x05000002);
}
static inline u8 *Ghost_GetDrawable(struct GhostActor *a)
{
    return a->drawable;
}
static inline u8 Ghost_GetAnimationIndex(struct GhostActor *a)
{
    return a->field2B7;
}
static inline void Ghost_SetAnimationIndex(struct GhostActor *a, u32 value)
{
    a->field2B7 = value;
}
static inline u8 Ghost_GetAnimationRestart(struct GhostActor *a)
{
    return a->field2B3;
}
static inline u16 Ghost_ReadKeywordHalfword(u32 keyword, s32 fallback)
{
    return Script_GetKeywordValue(keyword, fallback);
}
static inline s32 Value(s32 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline void Ghost_ReadScriptWord(u32 *output, u32 fallback)
{
    *output = Value(fallback);
}
static inline u32 Cell(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 Ghost_TerrainShape(const u8 *cell)
{
    return *cell >> 4;
}
static inline u32 Ghost_TerrainBaseHeight(const u8 *cell)
{
    return (*cell & 15) << 8;
}
static inline u32 Height(u32 cell, struct ContactPosition *p)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    u32 shape, height;
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = Ghost_TerrainShape(data);
    height = Ghost_TerrainBaseHeight(data);
    switch (shape) {
        case 1:
            height -= U8(p, 4);
            break;
        case 2:
            height -= U8(p, 0);
            break;
    }
    return height;
}
static inline void DrawablePosition(struct GhostActor *a)
{
    if (!Ghost_GetDrawableKind(a))
        POS(a->drawable, 0x1C) = a->body.position;
    else
        POS(a->drawable, 0x40) = a->body.position;
}
static inline u8 Ghost_ReadByte(u8 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline u8 Ghost_TestTargetingFlags(struct GhostTargeting *target, u32 mask)
{
    if (Ghost_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void Ghost_ReadTargeting(struct GhostTargeting *motion)
{
    {
        s32 x, angle, z;
        if (Script_SeekToKeyword('e')) {
            x = Value(0x30);
            angle = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
            z = x;
        } else {
            x = 0x30;
            z = 0x30;
            angle = 0x40;
        }
        motion->target = 0;
        motion->x = x;
        motion->z = z;
        motion->limit = x * x;
        motion->zSquared = z * z;
        motion->heading = angle;
        if (Ghost_TestTargetingFlags(motion, 4))
            motion->range = motion->limit;
        else
            motion->range = Div(motion->limit * 7, 10);
    }
}
static inline u16 Ghost_ReadHalfword(u16 fallback)
{
    if (Script_GetPc())
        return Script_GetValue();
    return fallback;
}
static inline void Ghost_SetFirstAttack(struct GhostActor *a, s32 index, s32 value)
{
    a->attacks.first[index] = value;
}
static inline struct GhostActor *Ghost_ReadAttacks(struct GhostActor *a)
{
    s32 i;
    if (Script_SeekToKeyword('a')) {
        i = 0;
        while (1) {
            Ghost_SetFirstAttack(a, i, Ghost_ReadHalfword(24));
            a->attacks.second[i] = Ghost_ReadHalfword(24);
            a->attacks.flags[i] = Value(0);
            a->attacks.parameter[i] = Ghost_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
        return a;
    }
    return a;
}
static inline s32 Ghost_ClampHealth(u16 raw)
{
    s32 health = (s16)raw;
    if (health <= 0)
        health = 0;
    return health;
}
static inline void Ghost_InstallIdle(struct GhostActor *a, void *idle)
{
    a->target = idle;
}
static inline void Ghost_SetInitialHealth(struct GhostActor *a, u16 health)
{
    a->defaultHealth = health;
}
static inline void Ghost_InitializeHealth(struct GhostActor *a, u16 initialHealth, s32 first,
                                          s32 second)
{
    a->flags2D4 = 0;
    Ghost_InstallIdle(a, SceneActorControl_OnIdle);
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->health = Ghost_ClampHealth(initialHealth);
    a->fieldAA = 0;
    if (second <= 0)
        second = first;
    a->fieldA6 = first;
    a->fieldA8 = second;
    a->field2FA = 0;
}
static inline void MonsterInit_WriteHalfword(u16 *slot, u32 value)
{
    *slot = value;
}
static inline void InitializeScriptFields(struct GhostActor *a, u32 *fill)
{
    struct GhostBody *body = &a->body;
    u32 id;
    struct ContactPosition origin;
    s32 first, second;
    s32 variant;
    a->flags2CC = 0;
    a->flags2D0 = 0;
    a->enabled = 0;
    a->field2E8 = 8;
    a->flags2C0 = Ghost_ReadKeywordHalfword('M', 0);
    Ghost_ClearPosition(fill, &origin);
    id = Script_GetKeywordValue('N', 0);
    Ghost_ReadTargeting(&Ghost_ReadAttacks(a)->targeting);
    if (Script_SeekToKeyword('l')) {
        first = Value(300);
        second = Value(300);
    } else {
        first = 0;
        second = 0;
    }
    if (Script_GetKeywordValue('b', 0))
        Ghost_SetStatusFlags(a, 0x200);
    a->field1F0 = Ghost_ReadKeywordHalfword('F', 0);
    a->field1EC = Ghost_ReadKeywordHalfword('R', 0);
    if (Script_SeekToKeyword('A')) {
        Ghost_SetInitialHealth(a, Ghost_ReadHalfword(0));
        a->field9E = Ghost_ReadByte(0);
        a->field9F = Ghost_ReadByte(0);
        a->fieldA0 = Ghost_ReadByte(0);
        a->fieldA2 = Ghost_ReadHalfword(0);
        a->fieldA4 = Ghost_ReadHalfword(0);
    }
    Ghost_InitializeHealth(a, Ghost_GetInitialHealth(a), first, second);
    if (Script_SeekToKeyword('X')) {
        a->fieldAC = Value(0);
        a->fieldA1 = Ghost_ReadByte(0);
    }
    MonsterInit_WriteHalfword(&a->fieldB2, Ghost_ReadKeywordHalfword('K', 2));
    variant = (s16)Script_GetKeywordValue('V', -1);
    if (variant < 0)
        variant = 0xFFFF;
    MonsterInit_WriteHalfword(&a->field9A, variant);
    if (Script_GetKeywordValue('H', 0))
        Ghost_SetStatusFlags(a, 0x800);
    SceneBody_Init(body, id, &origin, 0, 4, a);
    SceneBody_SetSprite(body, Ghost_GetDrawableKind(a) ? 0 : a->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)a + 0x40, 0x40, 0x40);
}
static inline void Ghost_ClearMotion(void *object)
{
    struct GhostActor *a = object;
    u32 zero = 0;
    a->word104 = zero;
    a->word108 = zero;
    a->word10C = zero;
    a->word110 = zero;
    a->flags114 = zero;
}
static inline void InstallPrimaryCallbacks(struct GhostActor *a, void *cb274, u32 (*cb260)(void *),
                                           void *cb224, void *cb228, void *cb22C, void *control,
                                           void *cb230, void *cb234, void *defaultReaction,
                                           void *targetQuery, void *damage, void *contact)
{
    u32 zero = 0;
    u32 mask = 0x2000;
    Ghost_SetBehaviorFlags(a, mask);
    a->field274 = cb274;
    a->field260 = cb260;
    a->entryTarget224 = cb224;
    a->entryTarget228 = cb228;
    a->savedTarget = cb22C;
    a->reactionTarget230 = cb230;
    a->field234 = cb234;
    a->field220 = control;
    a->field278 = defaultReaction;
    a->update270 = 0;
    a->field250 = 0;
    a->callback248 = 0;
    a->update26C = targetQuery;
    a->field238 = SceneActorReaction_SinkAndRemove;
    a->field264 = damage;
    a->field268 = contact;
    a->field240 = 0;
    Ghost_ClearMotion(a);
}
static inline void Ghost_ClearControlFlags(struct GhostActor *a, u32 mask)
{
    a->flags114 &= mask;
}
static inline void *Ghost_GetStandingControl(struct GhostActor *a)
{
    return a->savedTarget;
}
static inline void Ghost_SetControl(struct GhostActor *a, void *callback, u32 behavior)
{
    u32 zero = 0;
    u32 one = 1;
    a->field2B2 = one;
    a->field2B0 = zero;
    a->field2B5 = behavior;
    a->target = callback;
    a->field2C8 = zero;
    Ghost_ClearControlFlags(a, ~1);
    a->word104 = zero;
}
static inline void Ghost_SetState(struct GhostActor *a, u32 state, u32 phase, u32 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline u8 Ghost_IsInCurrentRegion(struct GhostActor *actor)
{
    if (gGameState->region == actor->field2FA)
        return 1;
    return 0;
}
static inline void InitializeCallbacks(struct GhostActor *a)
{
    SceneActor_InitLinkedReactionColliders(a);
    InstallPrimaryCallbacks(
        a, Ghost_OnBehaviorUpdate, Ghost_GetBehaviorCode, Ghost_TurnTowardHeading,
        Ghost_UpdateDirectionalPose, Ghost_UpdateDirectionalPoseB, 0, Ghost_UpdateHitStagger,
        Ghost_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0, SceneActorReaction_ResolveDamage,
        SceneActorPlacement_PlayResponseAndResetState);
    a->callback248 = Ghost_ResetMovementState;
    a->field23C = Ghost_OnControlCleanup;
    a->update270 = Ghost_GetControlStatus;
    a->field250 = (void *)gUnk_08605E14;
    Ghost_SetMotionFlags(a, 0x20);
    Ghost_SetBehaviorFlags(a, 0x200);
    Ghost_SetStatusFlags(a, 0x02100400);
    SceneActorBehavior_InitializeShadow(a);
    SceneActorTerrain_ReadScriptParameters(a);
    {
        u32 disabled = 0;
        if (Ghost_TestMotionFlags(a, 4)) {
            Ghost_SetStatusFlags(a, 0x2000);
            disabled = 1;
        }
        if (disabled != 1) {
            Ghost_SetState(a, 0, 0, 0);
            Ghost_SetControl(a, Ghost_GetStandingControl(a), 0);
        }
    }
    if (Ghost_TestMotionFlags(a, 2))
        a->field2FA = gGameState->region;
    else
        a->field2FA = Region_FindContaining(&a->body.position);
    if (Ghost_TestMotionFlags(a, 0x40)) {
        struct ContactCollider *collider = &a->colliderA;
        u32 mask = ~0x100;
        collider->flags = mask & collider->flags;
    }
}
struct GhostWidgetPalette {
    u8 unknown0[0x3A];
    u16 bank;
};
static inline void Ghost_SetWidgetPalette(struct GhostWidgetPalette *widget, s32 scriptPalette)
{
    widget->bank = scriptPalette - 0x5FFF;
}
static inline void Ghost_ReadPalette(struct GhostActor *a, u32 base, u32 alternate)
{
    s32 selected;
    a->paletteOffset = Ghost_ReadKeywordHalfword('C', 0);
    a->basePaletteBank = base;
    a->paletteAlternateBank = alternate;
    a->alternateOffset = Ghost_GetPaletteOffset(a) + base;
    selected = Ghost_GetPaletteBase(a) + Ghost_GetPaletteOffset(a);
    if (!Ghost_GetDrawableKind(a))
        AnimData_SelectPaletteBank((u8 *)a->drawable + 0x2C, selected);
    else
        PTR(a->drawable, 0x68) = gPaletteBankData + (selected << 5);
    if (Ghost_GetDrawableKind(a) == 1)
        Ghost_SetWidgetPalette((void *)(a->drawable + 0x20), SceneActorScript_GetManagerField62());
    a->alternatePalette = Ghost_GetPaletteBase(a);
    a->paletteBank = Ghost_GetPaletteBase(a) + 2;
    a->paletteFlashBank = 0x1EE;
}
static inline void Ghost_InitializeReceiver(struct GhostActor *a)
{
    a->field304 = 0;
    SceneActorRegistry_RegisterCategoryReceiver(a);
}
static inline void Ghost_InstallCompletion(struct GhostActor *a)
{
    a->field24C = SceneActorScript_OnCommand;
    a->field258 = Ghost_OnActorUpdate;
    a->field25C = Ghost_Destroy;
}
static inline void Ghost_SelectAnimation(struct GhostActor *actor, u32 selection, u32 variant,
                                         u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->field2B7 = selection;
        drawable = (void *)actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        selection += variant;
        if (!force && Ghost_GetAnimationIndex(actor) == selection &&
            !Ghost_GetAnimationRestart(actor)) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        Ghost_SetAnimationIndex(actor, selection);
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    actor->field2B3 = 0;
}

static inline void Ghost_InitializeDrawable(struct GhostActor *a, u32 *fill)
{
    struct ContactPosition zero;
    if (!Ghost_GetDrawableKind(a)) {
        u8 *d;
        a->drawable = Heap_Alloc(0x5C);
        Memory_Clear(a->drawable, 0x5C);
        d = Ghost_GetDrawable(a);
        AnimData_Load(d + 0x2C, 0xD637);
        Sprite_Init(d, d + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(d, 0x29FB);
        if (a->category == 0x11 || a->category == 0x17)
            d[7] = 1;
        a->body.direction = 0;
    } else {
        u8 *d;
        void *res;
        a->drawable = Heap_Alloc(0x80);
        Memory_Clear(a->drawable, 0x80);
        d = Ghost_GetDrawable(a);
        Ghost_ClearPosition(fill, &zero);
        res = ScriptProc_Start(0xCB05, 0xD637);
        *(struct ResourceCopy *)d = *(struct ResourceCopy *)res;
        ResourceHeader_Relocate(d, res);
        Widget_InitLinked(d + 0x20, d, 0, 0, 2, 2, 0x3C, &zero);
        a->field2B7 = 0xFF;
    }
}
static inline void Ghost_BeginRoute(struct GhostBody *body, struct GhostRoute *route)
{
    SceneBody_StartRoute(body, route, route->routeId, route->parameter, route->pointId);
}
static inline u8 Ghost_RouteActiveBranch(struct GhostRoute *route)
{
    return route->activeBranch;
}
static inline u8 Ghost_RouteId(struct GhostRoute *route)
{
    return route->routeId;
}
static inline u32 Ghost_AlternateRouteBranch(struct GhostRoute *route)
{
    return 1 ^ route->activeBranch;
}
static inline u32 Ghost_ReadRoute(struct GhostActor *a, struct GhostRoute *route)
{
    struct ContactPosition point;
    route->activeBranch = 0;
    if (Script_SeekToKeyword('r')) {
        route->routeId = Value(0);
        route->parameter = 0;
        route->pointId = Value(0);
        route->currentPoint = route->pointId;
        route->branches[Ghost_RouteActiveBranch(route)] = route->routeId;
        route->branches[Ghost_AlternateRouteBranch(route)] =
            Script_GetPc() ? Script_GetValue() : Ghost_RouteId(route);
        Path_GetPointPositionById((u16 *)&point, route->routeId, route->pointId);
        a->terrainCell = Cell(&point);
        point.y = a->groundHeight2BE = Height(Ghost_GetTerrainCell(a), &point);
        Ghost_BeginRoute(&a->body, route);
        return 1;
    } else {
        route->routeId = 0;
        route->parameter = 0;
        route->pointId = 0;
        route->currentPoint = 0;
        route->branches[Ghost_RouteActiveBranch(route)] = 0;
        route->branches[Ghost_AlternateRouteBranch(route)] = 0;
    }
    return 0;
}
s32 Ghost_Init(void *object)
{
    struct GhostActor *a = object;
    struct GhostActor *arg0 = object;
    u32 fill;
    u32 row, frameFlags;
    struct GhostRoute *route;
    struct SceneActorProfileEntry *profile;
    arg0->flags558 = 0;
    SceneActorRegistry_SetObjectContext(a, a, 0, 0);
    {
        u8 category = 0x17;
        a->category = category;
    }
    Ghost_InitActorProfile(a);
    Ghost_InitializeDrawable(a, &fill);
    InitializeScriptFields(a, &fill);
    route = &arg0->route;
    if (Ghost_ReadRoute(a, route)) {
        {
            struct ContactPosition *dest = &a->body.position;
            struct GhostRoute *link = route;
            dest->x = link->point->x;
            dest->z = link->point->z;
            dest->y = Height(Cell(dest), dest);
        }
        DrawablePosition(a);
        a->body.direction = Route_GetCurrentPointFacing(route);
    }
    if (Script_SeekToKeyword('p')) {
        a->body.position.x = Ghost_ReadHalfword(0);
        a->body.position.y = Ghost_ReadHalfword(0);
        a->body.position.z = Ghost_ReadHalfword(0);
        a->terrainCell = Cell(&a->body.position);
        a->groundHeight2BE = Height(Ghost_GetTerrainCell(a), &a->body.position);
        arg0->baseHeight = a->groundHeight2BE;
        DrawablePosition(a);
    } else
        arg0->baseHeight = a->groundHeight2BE;
    arg0->routeDelay = Ghost_ReadKeywordHalfword('l', 0);
    arg0->playerPosition = gPlayers[0]->position;
    if (Script_SeekToKeyword('P')) {
        arg0->phase560 = Ghost_ReadByte(0);
        arg0->phase561 = Ghost_ReadByte(0);
        arg0->phase5BC = Ghost_ReadByte(0);
    }
    if (Script_GetKeywordValue('g', 0))
        Ghost_SetInstanceFlags(arg0, 0x100);
    if (Script_SeekToKeyword('d')) {
        arg0->effectDuration = Ghost_ReadHalfword(0);
        Ghost_ReadScriptWord(&arg0->distanceSquared, 0);
        arg0->distanceSquared *= arg0->distanceSquared;
    } else {
        arg0->effectDuration = 0;
        arg0->distanceSquared = 0;
    }
    if (Script_GetKeywordValue('i', 0))
        Ghost_SetInstanceFlags(arg0, 0x200);
    Ghost_ReadPalette(a, 0x1EB, 0x192);
    if (!Script_GetKeywordValue('U', 0)) {
        struct GhostActor *context = a->context;
        s32 enabled;
        if (Ghost_TestInstanceFlags(context, 0x100))
            enabled = Ghost_TestWorldFlags(0x20);
        else
            enabled = gGameState->unk876;
        if (enabled > 0) {
            Ghost_SetInstanceFlags(context, 0x40);
            U32(a->drawable, 0) |= 1;
            context->visibilityTimer = 0;
        } else {
            Ghost_SetInstanceFlags(context, 0x20);
            U32(a->drawable, 0) &= ~1;
            context->visibilityTimer = 0;
        }
    }
    InitializeCallbacks(a);
    if (Script_GetKeywordValue('S', 0) || !Ghost_IsInCurrentRegion(a))
        SceneActorPlacement_HideActor(a, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    if (!Ghost_GetDrawableKind(a))
        DisplaySequence_Step(a->drawable);
    else
        Anim_Step((u8 *)a->drawable + 0x20, a->drawable);
    Ghost_InitializeReceiver(a);
    SceneActorBehavior_InstallCallbackTables(a);
    Ghost_InitMotionPreset(a);
    Ghost_InitActionTable(a);
    Ghost_InitReactionTable(a);
    if (arg0->effectDuration) {
        Ghost_InitEffectTable(a);
        Ghost_SetStatusFlags(a, 8);
    }
    profile = (void *)SceneActorProfiles_GetEntryAddress(a->profileId, a->profileMode, 1);
    Ghost_GetDirectionalFrame(a->body.direction, &row, &frameFlags);
    Ghost_SelectAnimation(a, profile->value, row, frameFlags);
    a->field2E8 = 2;
    SceneActor_InitLinkedSprites(a);
    Ghost_SaveControl(Ghost_SavedControlOf(a), a);
    a->saved.field130 = arg0->flags558;
    Ghost_InstallCompletion(a);
    if (!SceneActorRegistry_LinkActor(a))
        return -1;
    return 0;
}
