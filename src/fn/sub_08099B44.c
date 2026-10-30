#include "global.h"
#include "scene_actor_control_motion.h"
#include "display_sequence_inline.h"
#include "scene_actor_terrain.h"
#include "render.h"
#include "player.h"
#include "game_state.h"
#include "scene_actor_animation.h"
#include "scene_actor_init_primitives.h"
#define U8(p, n)  (*((u8 *)(p) + (n)))
#define U32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define PTR(p, n) (*(void **)((u8 *)(p) + (n)))
#define POS(p, n) (*(struct ContactPosition *)((u8 *)(p) + (n)))
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
struct GhostRoute {
    u32 unknown0, unknown4;
    struct GhostRoutePoint *point;
    u8 currentPoint, branches[2], activeBranch;
    u8 routeId, parameter, pointId, unknown13;
};
struct GhostInstanceState {
    u8 unknown0[0x558];
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
static inline u8 Ghost_GetDrawableKind(struct SceneActorRegistryResetView *a)
{
    return a->drawableKind;
}
static inline u16 Ghost_GetInitialHealth(struct SceneActorRegistryResetView *a)
{
    return a->defaultHealth;
}
static inline u16 Ghost_GetMotionFlags(struct SceneActorRegistryResetView *a)
{
    return a->flags2C0;
}
static inline u16 Ghost_GetHeightOffset(struct SceneActorRegistryResetView *a)
{
    return a->field2EA;
}
static inline u32 Ghost_GetBehaviorFlags(struct SceneActorRegistryResetView *a)
{
    return a->flags2CC;
}
static inline u32 Ghost_GetStatusFlags(struct SceneActorRegistryResetView *a)
{
    return a->flags2D0;
}
static inline u8 Ghost_GetDirection(struct SceneActorRegistryResetView *a)
{
    return a->body.direction;
}
static inline u8 Ghost_GetState(struct SceneActorRegistryResetView *a)
{
    return a->state2A8;
}
static inline u8 Ghost_GetPhase(struct SceneActorRegistryResetView *a)
{
    return a->phase2A9;
}
static inline u8 Ghost_GetBehavior(struct SceneActorRegistryResetView *a)
{
    return a->behavior2AA;
}
static inline void *Ghost_GetCurrentControl(struct SceneActorRegistryResetView *a)
{
    return a->target;
}
static inline u8 Ghost_GetControlKind(struct SceneActorRegistryResetView *a)
{
    return a->field2B5;
}
static inline u32 Ghost_GetPaletteBase(struct SceneActorRegistryResetView *a)
{
    return a->basePaletteBank;
}
static inline u32 Ghost_GetPaletteOffset(struct SceneActorRegistryResetView *a)
{
    return a->paletteOffset;
}
static inline u32 Ghost_GetTerrainCell(struct SceneActorRegistryResetView *a)
{
    return a->terrainCell;
}
static inline void Ghost_SetInstanceFlags(struct GhostInstanceState *a, u32 mask)
{
    a->flags558 |= mask;
}
static inline u8 Ghost_TestInstanceFlags(struct GhostInstanceState *a, u32 mask)
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
static inline struct SceneActorDefaultControl *Ghost_SavedControlOf(struct SceneActorRegistryResetView *a)
{
    return &a->saved;
}
static inline void Ghost_SetMotionFlags(struct SceneActorRegistryResetView *a, u16 mask)
{
    a->flags2C0 |= mask;
}
static inline u8 Ghost_TestMotionFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    if (Ghost_GetMotionFlags(a) & mask)
        return 1;
    return 0;
}
static inline void Ghost_SetBehaviorFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->flags2CC |= mask;
}
static inline void Ghost_SetStatusFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->flags2D0 |= mask;
}
static inline u32 Ghost_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline void Ghost_SaveControl(struct SceneActorDefaultControl *saved, struct SceneActorRegistryResetView *a)
{
    saved->motionFlags = Ghost_GetMotionFlags(a);
    saved->heightOffset = Ghost_GetHeightOffset(a);
    saved->behaviorFlags = Ghost_GetBehaviorFlags(a);
    saved->statusFlags = Ghost_GetStatusFlags(a);
    saved->direction = Ghost_GetDirection(a);
    saved->state = Ghost_GetState(a);
    saved->phase = Ghost_GetPhase(a);
    saved->behavior = Ghost_GetBehavior(a);
    saved->control = Ghost_GetCurrentControl(a);
    saved->kind = Ghost_GetControlKind(a);
}
static inline u8 *Ghost_GetDrawable(struct SceneActorRegistryResetView *a)
{
    return a->drawable;
}
static inline u8 Ghost_GetAnimationIndex(struct SceneActorRegistryResetView *a)
{
    return a->animationIndex;
}
static inline void Ghost_SetAnimationIndex(struct SceneActorRegistryResetView *a, u32 value)
{
    a->animationIndex = value;
}
static inline u8 Ghost_GetAnimationRestart(struct SceneActorRegistryResetView *a)
{
    return a->field2B3;
}
static inline void DrawablePosition(struct SceneActorRegistryResetView *a)
{
    if (!Ghost_GetDrawableKind(a))
        POS(a->drawable, 0x1C) = a->body.position;
    else
        POS(a->drawable, 0x40) = a->body.position;
}
static inline u8 Ghost_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (Ghost_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void Ghost_ReadTargeting(struct SceneActorChaseTracking *motion)
{
    {
        s32 x, angle, z;
        if (Script_SeekToKeyword('e')) {
            x = MonsterInit_ReadWord(0x30);
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
        motion->fieldEA = angle;
        if (Ghost_TestTargetingFlags(motion, 4))
            motion->range = motion->limit;
        else
            motion->range = Div(motion->limit * 7, 10);
    }
}
static inline void Ghost_SetFirstAttack(struct SceneActorRegistryResetView *a, s32 index, s32 value)
{
    a->attacks.first[index] = value;
}
static inline struct SceneActorRegistryResetView *Ghost_ReadAttacks(struct SceneActorRegistryResetView *a)
{
    s32 i;
    if (Script_SeekToKeyword('a')) {
        i = 0;
        while (1) {
            Ghost_SetFirstAttack(a, i, MonsterInit_ReadHalfword(24));
            a->attacks.second[i] = MonsterInit_ReadHalfword(24);
            a->attacks.flags[i] = MonsterInit_ReadWord(0);
            a->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
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
static inline void Ghost_InstallIdle(struct SceneActorRegistryResetView *a, void *idle)
{
    a->target = idle;
}
static inline void Ghost_SetInitialHealth(struct SceneActorRegistryResetView *a, u16 health)
{
    a->defaultHealth = health;
}
static inline void Ghost_InitializeHealth(struct SceneActorRegistryResetView *a, u16 initialHealth, s32 first,
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
    a->attachmentLimit = first;
    a->fieldA8 = second;
    a->regionId = 0;
}
static inline void MonsterInit_WriteHalfword(u16 *slot, u32 value)
{
    *slot = value;
}
static inline void InitializeScriptFields(struct SceneActorRegistryResetView *a, u32 *fill)
{
    struct SceneActorBody *body = &a->body;
    u32 id;
    struct ContactPosition origin;
    s32 first, second;
    s32 variant;
    a->flags2CC = 0;
    a->flags2D0 = 0;
    a->enabled = 0;
    a->field2E8 = 8;
    a->flags2C0 = MonsterInit_ReadKeywordHalfword('M', 0);
    MonsterInit_ClearPosition(fill, &origin);
    id = Script_GetKeywordValue('N', 0);
    Ghost_ReadTargeting(&Ghost_ReadAttacks(a)->tracking);
    if (Script_SeekToKeyword('l')) {
        first = MonsterInit_ReadWord(300);
        second = MonsterInit_ReadWord(300);
    } else {
        first = 0;
        second = 0;
    }
    if (Script_GetKeywordValue('b', 0))
        Ghost_SetStatusFlags(a, 0x200);
    a->field1F0 = MonsterInit_ReadKeywordHalfword('F', 0);
    a->field1EC = MonsterInit_ReadKeywordHalfword('R', 0);
    if (Script_SeekToKeyword('A')) {
        Ghost_SetInitialHealth(a, MonsterInit_ReadHalfword(0));
        a->field9E = MonsterInit_ReadByte(0);
        a->field9F = MonsterInit_ReadByte(0);
        a->fieldA0 = MonsterInit_ReadByte(0);
        a->fieldA2 = MonsterInit_ReadHalfword(0);
        a->fieldA4 = MonsterInit_ReadHalfword(0);
    }
    Ghost_InitializeHealth(a, Ghost_GetInitialHealth(a), first, second);
    if (Script_SeekToKeyword('X')) {
        a->fieldAC = MonsterInit_ReadWord(0);
        a->fieldA1 = MonsterInit_ReadByte(0);
    }
    MonsterInit_WriteHalfword(&a->fieldB2, MonsterInit_ReadKeywordHalfword('K', 2));
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
    struct SceneActorRegistryResetView *a = object;
    u32 zero = 0;
    a->word104 = zero;
    a->word108 = zero;
    a->word10C = zero;
    a->word110 = zero;
    a->flags114 = zero;
}
static inline void InstallPrimaryCallbacks(struct SceneActorRegistryResetView *a, void *cb274, u32 (*cb260)(void *),
                                           void *cb224, void *cb228, void *cb22C, void *control,
                                           void *cb230, void *cb234, void *defaultReaction,
                                           void *targetQuery, void *damage, void *contact)
{
    u32 zero = 0;
    u32 mask = 0x2000;
    Ghost_SetBehaviorFlags(a, mask);
    a->contextCallback = cb274;
    {
        SceneActorAnimationQuery *query = &a->animationQuery.word;
        *query = cb260;
    }
    a->entryTarget224 = cb224;
    a->entryTarget228 = cb228;
    a->savedTarget = cb22C;
    a->reactionTarget230 = cb230;
    a->deathControl = cb234;
    a->field220 = control;
    a->defaultReaction = defaultReaction;
    {
        void **query = &a->contactQuery.opaque;
        *query = 0;
    }
    {
        const void **commands = &a->commandTable.opaque;
        *commands = 0;
    }
    a->callback248 = 0;
    a->update26C = targetQuery;
    a->field238 = SceneActorReaction_SinkAndRemove;
    a->damageCallback = damage;
    a->contactCallback = contact;
    a->field240 = 0;
    Ghost_ClearMotion(a);
}
static inline void Ghost_ClearControlFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    a->flags114 &= mask;
}
static inline void *Ghost_GetStandingControl(struct SceneActorRegistryResetView *a)
{
    return a->savedTarget;
}
static inline void Ghost_SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 behavior)
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
static inline void Ghost_SetState(struct SceneActorRegistryResetView *a, u32 state, u32 phase, u32 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline u8 Ghost_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (gGameState->region == actor->regionId)
        return 1;
    return 0;
}
static inline void InitializeCallbacks(struct SceneActorRegistryResetView *a)
{
    SceneActor_InitLinkedReactionColliders(a);
    InstallPrimaryCallbacks(
        a, Ghost_OnBehaviorUpdate, Ghost_GetBehaviorCode, Ghost_TurnTowardHeading,
        Ghost_UpdateDirectionalPose, Ghost_UpdateDirectionalPoseB, 0, Ghost_UpdateHitStagger,
        Ghost_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0, SceneActorReaction_ResolveDamage,
        SceneActorPlacement_PlayResponseAndResetState);
    a->callback248 = Ghost_ResetMovementState;
    a->field23C = Ghost_OnControlCleanup;
    {
        void **query = &a->contactQuery.opaque;
        *query = Ghost_GetControlStatus;
    }
    {
        const void **commands = &a->commandTable.opaque;
        *commands = gUnk_08605E14;
    }
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
        a->regionId = gGameState->region;
    else
        a->regionId = Region_FindContaining(&a->body.position);
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
static inline void Ghost_ReadPalette(struct SceneActorRegistryResetView *a, u32 base, u32 alternate)
{
    s32 selected;
    a->paletteOffset = MonsterInit_ReadKeywordHalfword('C', 0);
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
    a->field1EA = 0x1EE;
}
static inline void Ghost_InitializeReceiver(struct SceneActorRegistryResetView *a)
{
    a->field304 = 0;
    SceneActorRegistry_RegisterCategoryReceiver(a);
}
static inline void Ghost_InstallCompletion(struct SceneActorRegistryResetView *a)
{
    a->messageCallback = SceneActorScript_OnCommand;
    a->updateCallback = Ghost_OnActorUpdate;
    a->destroyCallback = Ghost_Destroy;
}
static inline void Ghost_SelectAnimation(struct SceneActorRegistryResetView *actor, u32 selection, u32 variant,
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
        actor->animationIndex = selection;
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

static inline void Ghost_InitializeDrawable(struct SceneActorRegistryResetView *a, u32 *fill)
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
        MonsterInit_ClearPosition(fill, &zero);
        res = ScriptProc_Start(0xCB05, 0xD637);
        *(struct ResourceCopy *)d = *(struct ResourceCopy *)res;
        ResourceHeader_Relocate(d, res);
        Widget_InitLinked(d + 0x20, d, 0, 0, 2, 2, 0x3C, &zero);
        a->animationIndex = 0xFF;
    }
}
static inline void Ghost_BeginRoute(struct SceneActorBody *body, struct GhostRoute *route)
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
static inline u32 Ghost_RoutePointXRaw(struct GhostRoute *route)
{
    return (u16)route->point->x;
}
static inline u32 Ghost_RoutePointZRaw(struct GhostRoute *route)
{
    return (u16)route->point->z;
}
static inline u32 Ghost_ReadRoute(struct SceneActorRegistryResetView *a, struct GhostRoute *route)
{
    struct ContactPosition point;
    route->activeBranch = 0;
    if (Script_SeekToKeyword('r')) {
        route->routeId = MonsterInit_ReadWord(0);
        route->parameter = 0;
        route->pointId = MonsterInit_ReadWord(0);
        route->currentPoint = route->pointId;
        route->branches[Ghost_RouteActiveBranch(route)] = route->routeId;
        route->branches[Ghost_AlternateRouteBranch(route)] =
            Script_GetPc() ? Script_GetValue() : Ghost_RouteId(route);
        Path_GetPointPositionById((u16 *)&point, route->routeId, route->pointId);
        a->terrainCell = MonsterInit_CellIndex(&point);
        point.y = a->groundHeight2BE = MonsterInit_TerrainHeight(Ghost_GetTerrainCell(a), &point);
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
    struct SceneActorRegistryResetView *a = object;
    struct GhostInstanceState *arg0 = object;
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
            dest->x = Ghost_RoutePointXRaw(link);
            dest->z = Ghost_RoutePointZRaw(link);
            dest->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(dest), dest);
        }
        DrawablePosition(a);
        a->body.direction = Route_GetCurrentPointFacing(route);
    }
    if (Script_SeekToKeyword('p')) {
        a->body.position.x = MonsterInit_ReadHalfword(0);
        a->body.position.y = MonsterInit_ReadHalfword(0);
        a->body.position.z = MonsterInit_ReadHalfword(0);
        a->terrainCell = MonsterInit_CellIndex(&a->body.position);
        a->groundHeight2BE = MonsterInit_TerrainHeight(Ghost_GetTerrainCell(a), &a->body.position);
        arg0->baseHeight = a->groundHeight2BE;
        DrawablePosition(a);
    } else
        arg0->baseHeight = a->groundHeight2BE;
    arg0->routeDelay = MonsterInit_ReadKeywordHalfword('l', 0);
    arg0->playerPosition = gPlayers[0]->position;
    if (Script_SeekToKeyword('P')) {
        arg0->phase560 = MonsterInit_ReadByte(0);
        arg0->phase561 = MonsterInit_ReadByte(0);
        arg0->phase5BC = MonsterInit_ReadByte(0);
    }
    if (Script_GetKeywordValue('g', 0))
        Ghost_SetInstanceFlags(arg0, 0x100);
    if (Script_SeekToKeyword('d')) {
        arg0->effectDuration = MonsterInit_ReadHalfword(0);
        MonsterInit_ReadScriptWord(&arg0->distanceSquared, 0);
        arg0->distanceSquared *= arg0->distanceSquared;
    } else {
        arg0->effectDuration = 0;
        arg0->distanceSquared = 0;
    }
    if (Script_GetKeywordValue('i', 0))
        Ghost_SetInstanceFlags(arg0, 0x200);
    Ghost_ReadPalette(a, 0x1EB, 0x192);
    if (!Script_GetKeywordValue('U', 0)) {
        struct GhostInstanceState *context = a->context;
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
    a->saved.privateFlags = arg0->flags558;
    Ghost_InstallCompletion(a);
    if (!SceneActorRegistry_LinkActor(a))
        return -1;
    return 0;
}
