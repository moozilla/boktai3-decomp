#include "global.h"
#include "scene_actor_control_motion.h"
#include "display_sequence_inline.h"
#include "scene_actor_terrain.h"
#include "render.h"
#include "player.h"
#include "game_state.h"
#include "scene_actor_animation.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"
#define U8(p, n)  (*((u8 *)(p) + (n)))
#define U32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define PTR(p, n) (*(void **)((u8 *)(p) + (n)))
void SceneActorRegistry_SetObjectContext(void *, void *, u32, void *);
void Ghost_InitActorProfile(void *);
void SceneBody_Init(void *, u32, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, u32, u32);
s32 Route_GetCurrentPointFacing(void *);
s32 SceneActorScript_GetManagerField62(void);
void SceneActor_InitLinkedReactionColliders(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
void SceneActorPlacement_HideActor(void *, u32);
void SceneActorBehavior_InstallCallbackTables(void *);
void Ghost_InitMotionPreset(void *);
void Ghost_InitActionTable(void *);
void Ghost_InitReactionTable(void *);
void Ghost_InitEffectTable(void *);
void Ghost_GetDirectionalFrame(u8, u32 *, u32 *);
void SceneActor_InitLinkedSprites(void *);
u8 SceneActorRegistry_LinkActor(void *);
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
    struct SceneActorRegistryRoute route;
    u8 unknown620[2];
    u16 routeDelay;
};
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
    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(a)->tracking);
    if (Script_SeekToKeyword('l')) {
        first = MonsterInit_ReadWord(300);
        second = MonsterInit_ReadWord(300);
    } else {
        first = 0;
        second = 0;
    }
    if (Script_GetKeywordValue('b', 0))
        MonsterInit_SetStatusFlags(a, 0x200);
    a->field1F0 = MonsterInit_ReadKeywordHalfword('F', 0);
    a->field1EC = MonsterInit_ReadKeywordHalfword('R', 0);
    if (Script_SeekToKeyword('A')) {
        MonsterInit_SetInitialHealth(a, MonsterInit_ReadHalfword(0));
        a->field9E = MonsterInit_ReadByte(0);
        a->field9F = MonsterInit_ReadByte(0);
        a->fieldA0 = MonsterInit_ReadByte(0);
        a->fieldA2 = MonsterInit_ReadHalfword(0);
        a->fieldA4 = MonsterInit_ReadHalfword(0);
    }
    MonsterInit_InitializeHealth(a, MonsterInit_GetInitialHealth(a), first, second);
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
        MonsterInit_SetStatusFlags(a, 0x800);
    SceneBody_Init(body, id, &origin, 0, 4, a);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(a) ? 0 : a->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)a + 0x40, 0x40, 0x40);
}
static inline void InstallPrimaryCallbacks(struct SceneActorRegistryResetView *a, void *cb274,
                                           u32 (*cb260)(void *), void *cb224, void *cb228,
                                           void *cb22C, void *control, void *cb230, void *cb234,
                                           void *defaultReaction, void *targetQuery, void *damage,
                                           void *contact)
{
    u32 zero = 0;
    u32 mask = 0x2000;
    MonsterInit_SetBehaviorFlags(a, mask);
    a->contextCallback = cb274;
    MonsterInit_WriteAnimationQuery(&a->animationQuery.word, cb260);
    a->entryTarget224 = cb224;
    a->entryTarget228 = cb228;
    a->savedTarget = cb22C;
    a->reactionTarget230 = cb230;
    a->deathControl = cb234;
    a->field220 = control;
    a->defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&a->contactQuery.opaque, 0);
    MonsterInit_WriteDataPointer(&a->commandTable.opaque, 0);
    a->callback248 = 0;
    a->update26C = targetQuery;
    a->field238 = SceneActorReaction_SinkAndRemove;
    a->damageCallback = damage;
    a->contactCallback = contact;
    a->field240 = 0;
    MonsterInit_ClearMotion(a);
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
    MonsterInit_WriteCallback(&a->contactQuery.opaque, Ghost_GetControlStatus);
    MonsterInit_WriteDataPointer(&a->commandTable.opaque, gUnk_08605E14);
    MonsterInit_SetMotionFlags(a, 0x20);
    MonsterInit_SetBehaviorFlags(a, 0x200);
    MonsterInit_SetStatusFlags(a, 0x02100400);
    SceneActorBehavior_InitializeShadow(a);
    SceneActorTerrain_ReadScriptParameters(a);
    {
        u32 disabled = 0;
        if (MonsterInit_TestMotionFlags(a, 4)) {
            MonsterInit_SetStatusFlags(a, 0x2000);
            disabled = 1;
        }
        if (disabled != 1) {
            MonsterInit_SetState(a, 0, 0, 0);
            MonsterInit_SetControl(a, MonsterInit_GetStandingControl(a), 0);
        }
    }
    if (MonsterInit_TestMotionFlags(a, 2))
        a->regionId = gGameState->region;
    else
        a->regionId = Region_FindContaining(&a->body.position);
    if (MonsterInit_TestMotionFlags(a, 0x40)) {
        struct ContactCollider *collider = &a->colliderA;
        u32 mask = ~0x100;
        collider->flags = mask & collider->flags;
    }
}

static inline void Ghost_ReadPalette(struct SceneActorRegistryResetView *a, u32 base, u32 alternate)
{
    s32 selected;
    a->paletteOffset = MonsterInit_ReadKeywordHalfword('C', 0);
    a->basePaletteBank = base;
    a->paletteAlternateBank = alternate;
    a->alternateOffset = MonsterInit_GetPaletteOffset(a) + base;
    selected = MonsterInit_GetPaletteBase(a) + MonsterInit_GetPaletteOffset(a);
    if (!MonsterInit_GetDrawableKind(a))
        AnimData_SelectPaletteBank((u8 *)a->drawable + 0x2C, selected);
    else
        PTR(a->drawable, 0x68) = gPaletteBankData + (selected << 5);
    if (MonsterInit_GetDrawableKind(a) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(a->drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
    a->alternatePalette = MonsterInit_GetPaletteBase(a);
    a->paletteBank = MonsterInit_GetPaletteBase(a) + 2;
    a->field1EA = 0x1EE;
}
static inline void Ghost_InstallCompletion(struct SceneActorRegistryResetView *a)
{
    a->messageCallback = SceneActorScript_OnCommand;
    a->updateCallback = Ghost_OnActorUpdate;
    a->destroyCallback = Ghost_Destroy;
}
static inline void Ghost_SelectAnimation(struct SceneActorRegistryResetView *actor, u32 selection,
                                         u32 variant, u32 flags)
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
    if (!MonsterInit_GetDrawableKind(a)) {
        u8 *d;
        a->drawable = Heap_Alloc(0x5C);
        Memory_Clear(a->drawable, 0x5C);
        d = MonsterInit_Drawable(a);
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
        d = MonsterInit_Drawable(a);
        MonsterInit_ClearPosition(fill, &zero);
        res = ScriptProc_Start(0xCB05, 0xD637);
        *(struct DisplayResourceHeader *)d = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(d, res);
        Widget_InitLinked(d + 0x20, d, 0, 0, 2, 2, 0x3C, &zero);
        a->animationIndex = 0xFF;
    }
}
static inline u32 Ghost_ReadRoute(struct SceneActorRegistryResetView *a,
                                  struct SceneActorRegistryRoute *route)
{
    struct ContactPosition point;
    route->activeBranch = 0;
    if (Script_SeekToKeyword('r')) {
        route->routeId = MonsterInit_ReadWord(0);
        route->parameterB = 0;
        route->pointId = MonsterInit_ReadWord(0);
        route->currentPoint = route->pointId;
        route->branches[MonsterInit_RouteActiveBranch(route)] = route->routeId;
        route->branches[MonsterInit_RouteAlternateBranch(route)] =
            Script_GetPc() ? Script_GetValue() : MonsterInit_RouteId(route);
        Path_GetPointPositionById((u16 *)&point, route->routeId, route->pointId);
        a->terrainCell = MonsterInit_CellIndex(&point);
        point.y = a->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(a), &point);
        MonsterInit_BeginRoute(&a->body, route);
        return 1;
    } else {
        route->routeId = 0;
        route->parameterB = 0;
        route->pointId = 0;
        route->currentPoint = 0;
        route->branches[MonsterInit_RouteActiveBranch(route)] = 0;
        route->branches[MonsterInit_RouteAlternateBranch(route)] = 0;
    }
    return 0;
}
s32 Ghost_Init(void *object)
{
    struct SceneActorRegistryResetView *a = object;
    struct GhostInstanceState *arg0 = object;
    u32 fill;
    u32 row, frameFlags;
    struct SceneActorRegistryRoute *route;
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
            struct SceneActorRegistryRoute *link = route;
            dest->x = MonsterInit_RoutePointXRaw(link);
            dest->z = MonsterInit_RoutePointZRaw(link);
            dest->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(dest), dest);
        }
        MonsterInit_DrawablePosition(a);
        a->body.direction = Route_GetCurrentPointFacing(route);
    }
    if (Script_SeekToKeyword('p')) {
        a->body.position.x = MonsterInit_ReadHalfword(0);
        a->body.position.y = MonsterInit_ReadHalfword(0);
        a->body.position.z = MonsterInit_ReadHalfword(0);
        a->terrainCell = MonsterInit_CellIndex(&a->body.position);
        a->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(a), &a->body.position);
        arg0->baseHeight = a->groundHeight2BE;
        MonsterInit_DrawablePosition(a);
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
            enabled = MonsterInit_TestWorldFlags(0x20);
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
    if (Script_GetKeywordValue('S', 0) || !MonsterInit_IsInCurrentRegion(a))
        SceneActorPlacement_HideActor(a, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    if (!MonsterInit_GetDrawableKind(a))
        DisplaySequence_Step(a->drawable);
    else
        Anim_Step((u8 *)a->drawable + 0x20, a->drawable);
    MonsterInit_InitializeReceiver(a);
    SceneActorBehavior_InstallCallbackTables(a);
    Ghost_InitMotionPreset(a);
    Ghost_InitActionTable(a);
    Ghost_InitReactionTable(a);
    if (arg0->effectDuration) {
        Ghost_InitEffectTable(a);
        MonsterInit_SetStatusFlags(a, 8);
    }
    profile = (void *)SceneActorProfiles_GetEntryAddress(a->profileId, a->profileMode, 1);
    Ghost_GetDirectionalFrame(a->body.direction, &row, &frameFlags);
    Ghost_SelectAnimation(a, profile->value, row, frameFlags);
    a->field2E8 = 2;
    SceneActor_InitLinkedSprites(a);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(a), a);
    a->saved.privateFlags = arg0->flags558;
    Ghost_InstallCompletion(a);
    if (!SceneActorRegistry_LinkActor(a))
        return -1;
    return 0;
}
