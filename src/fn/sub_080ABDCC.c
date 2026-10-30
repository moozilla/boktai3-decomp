#include "global.h"
#include "scene_actor_state.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_actor.h"
typedef void (*MonsterInitCommandHandler)(void *actor, const u16 *command);
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void TentacleActor_InitProfile(void *);
void SceneActorControl_OnIdle(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorScript_OnCommand(void);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);
s32 Region_FindContaining(void *);

extern MonsterInitCommandHandler const gUnk_086108B0[];
extern u8 *gGameState;
struct MonsterInit32Block {
    u32 w[8];
};
struct TentacleActor {
    u8 unknown0[0x3D4];
    u8 unknown3D4[0x4FC];
    u16 flags;
    u8 unknown8D2[0x14];
    u16 interval8E6;
    u8 unknown8E8[0x44];
    struct ContactPosition savedPosition;
};
void TentacleActor_OnUpdate(void *);
u32 TentacleActor_OnResolveVariantValue(void *);
void TentacleBody_OnIdleState(void *);
void TentacleBody_UpdateAttackWindup(void *);
void TentacleBody_ResetDirectionalPose(void *);
void sub_080A71A0(void *);
void TentacleBody_UpdateDefeat(void *);
void TentacleBody_UpdateHiddenTransition(void *);
void sub_080ABB18(void *);
void ActorTrail_ResetAtSavedPosition(void *);
void SceneActor_ResetQueuedState(void *);
s32 SceneActor_TryRequestFacingReaction(void *);
void sub_080AA744(void *);
void TentacleEnemy_InitPlayerReference(void *);
void TentacleBody_Destroy(void *);
void TentacleActor_ResetRouteHistory(void *);
void TentacleActor_InitCollision(void *);
void TentacleActor_InitSegmentUpdateState(void *);
void TentacleActor_SelectWanderRoute(void *);
void TentacleBody_InitBehaviorTable(void *);
void TentacleActor_InitStateHandlers(void *);
void TentacleBody_InitSegmentAnimations(void *, u32, u32, u32);
void TentacleActor_ResetSegmentPositions(void *, void *);
void TentacleActor_InitSegmentColliders(void *);
void TentacleActor_ActivateMarkedSegment(void *);

static inline u8 MonsterInit_GetDrawableKind(struct SceneActorRegistryResetView *actor)
{
    return actor->drawableKind;
}
static inline u8 MonsterInit_GetCategory(struct SceneActorRegistryResetView *actor)
{
    return actor->category;
}

static inline u16 MonsterInit_GetInitialHealth(struct SceneActorRegistryResetView *actor)
{
    return actor->defaultHealth;
}

static inline u32 MonsterInit_GetPaletteBase(struct SceneActorRegistryResetView *actor)
{
    return actor->basePaletteBank;
}

static inline u32 MonsterInit_GetPaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->paletteOffset;
}
#define SV(d) MonsterInit_ReadWord(d)
static inline u16 MonsterInit_GetMotionFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2C0;
}
static inline u16 MonsterInit_GetHeightOffset(struct SceneActorRegistryResetView *actor)
{
    return actor->field2EA;
}
static inline u32 MonsterInit_GetBehaviorFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2CC;
}
static inline u32 MonsterInit_GetStatusFlags(struct SceneActorRegistryResetView *actor)
{
    return actor->flags2D0;
}
static inline u8 MonsterInit_GetState(struct SceneActorRegistryResetView *actor)
{
    return actor->state2A8;
}
static inline u8 MonsterInit_GetPhase(struct SceneActorRegistryResetView *actor)
{
    return actor->phase2A9;
}
static inline u8 MonsterInit_GetBehavior(struct SceneActorRegistryResetView *actor)
{
    return actor->behavior2AA;
}
static inline void *MonsterInit_GetCurrentControl(struct SceneActorRegistryResetView *actor)
{
    return actor->target;
}
static inline u8 MonsterInit_GetControlKind(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B5;
}
static inline u8 MonsterInit_GetDirection(struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}
static inline u16 MonsterInit_GetTerrainCell(struct SceneActorRegistryResetView *actor)
{
    return actor->terrainCell;
}
static inline u32 MonsterInit_GetTargetingFlags(struct SceneActorChaseTracking *target)
{
    return target->flags;
}
static inline struct SceneActorDefaultControl *
MonsterInit_SavedControlOf(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorDefaultControl *saved = &actor->saved;
    return saved;
}

static inline void MonsterInit_SaveControl(struct SceneActorDefaultControl *saved,
                                           struct SceneActorRegistryResetView *actor)
{
    saved->motionFlags = MonsterInit_GetMotionFlags(actor);
    saved->heightOffset = MonsterInit_GetHeightOffset(actor);
    saved->behaviorFlags = MonsterInit_GetBehaviorFlags(actor);
    saved->statusFlags = MonsterInit_GetStatusFlags(actor);
    saved->direction = MonsterInit_GetDirection(actor);
    saved->state = MonsterInit_GetState(actor);
    saved->phase = MonsterInit_GetPhase(actor);
    saved->behavior = MonsterInit_GetBehavior(actor);
    saved->control = MonsterInit_GetCurrentControl(actor);
    saved->kind = MonsterInit_GetControlKind(actor);
}
static inline void MonsterInit_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 |= mask;
}
static inline void MonsterInit_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2C0 |= mask;
}
static inline void *MonsterInit_GetIdleControl(struct SceneActorRegistryResetView *actor)
{
    return actor->savedTarget;
}
static inline u8 MonsterInit_TestMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (MonsterInit_GetMotionFlags(actor) & mask)
        return 1;
    return 0;
}
static inline u8 MonsterInit_TestTargetingFlags(struct SceneActorChaseTracking *target, u32 mask)
{
    if (MonsterInit_GetTargetingFlags(target) & mask)
        return 1;
    return 0;
}
static inline void MonsterInit_ClearControlFlags(struct SceneActorRegistryResetView *actor,
                                                 u32 mask)
{
    actor->flags114 &= mask;
}
static inline u8 MonsterInit_IsInCurrentRegion(struct SceneActorRegistryResetView *actor)
{
    if (*(u32 *)(gGameState + 0x5A0) == actor->regionId)
        return 1;
    return 0;
}
static inline void MonsterInit_SetControl(struct SceneActorRegistryResetView *actor, void *callback,
                                          u32 behavior)
{
    u32 zero = 0;
    u32 one = 1;
    actor->field2B2 = one;
    actor->field2B0 = zero;
    actor->field2B5 = behavior;
    actor->target = callback;
    actor->field2C8 = zero;
    MonsterInit_ClearControlFlags(actor, ~1);
    actor->word104 = zero;
}
static inline void MonsterInit_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                        u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void MonsterInit_ReadTargeting(struct SceneActorChaseTracking *target)
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
    target->target = 0;
    target->x = r4v;
    target->z = r1v;
    target->limit = r4v * r4v;
    target->zSquared = r1v * r1v;
    target->fieldEA = r2v;
    if (MonsterInit_TestTargetingFlags(target, 4))
        target->range = target->limit;
    else
        target->range = Div(target->limit * 8 - target->limit, 10);
}

struct MonsterInitCollider {
    u8 header[6];
    u16 flags;
};
static inline void MonsterInit_ClearColliderFlags(struct MonsterInitCollider *collider, u32 mask)
{
    collider->flags = mask & collider->flags;
}
struct MonsterInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MonsterInit_SetWidgetPalette(struct MonsterInitWidget *widget, u32 palette)
{
    widget->paletteBank = palette;
}
static inline void MonsterInit_SetFirstAttack(struct SceneActorRegistryResetView *actor, s32 index,
                                              s32 value)
{
    actor->attacks.first[index] = value;
}
static inline struct SceneActorRegistryResetView *
MonsterInit_ReadAttacks(struct SceneActorRegistryResetView *actor)
{
    s32 i;
    if (Script_SeekToKeyword('a')) {
        i = 0;
        while (1) {
            MonsterInit_SetFirstAttack(actor, i, MonsterInit_ReadHalfword(24));
            actor->attacks.second[i] = MonsterInit_ReadHalfword(24);
            actor->attacks.flags[i] = MonsterInit_ReadWord(0);
            actor->attacks.parameter[i] = MonsterInit_ReadHalfword(0);
            i++;
            if (i > 3)
                break;
        }
        return actor;
    }
    return actor;
}

static inline s32 MonsterInit_ClampHealth(u16 initialHealth)
{
    s32 health = (s16)initialHealth;
    if (health <= 0)
        health = 0;
    return health;
}
static inline void MonsterInit_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, r4v, r6v, r3v;
    s32 initialHealth, scriptVariant;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = Script_GetKeywordValue(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->tracking);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = MonsterInit_ReadWord(0x12C);
        r4v = MonsterInit_ReadWord(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = (u16)Script_GetKeywordValue(0x46, 0);
    arg0->field1EC = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        arg0->defaultHealth = MonsterInit_ReadHalfword(0);
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    initialHealth = MonsterInit_GetInitialHealth(arg0);
    r3v = r4v;
    arg0->flags2D4 = 0;
    arg0->target = SceneActorControl_OnIdle;
    arg0->field2B2 = 1;
    arg0->field2B0 = 0;
    arg0->health = MonsterInit_ClampHealth(initialHealth);
    arg0->fieldAA = 0;
    if (r3v <= 0)
        r3v = r6v;
    arg0->attachmentLimit = r6v;
    arg0->fieldA8 = r3v;
    arg0->regionId = 0;

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    scriptVariant = (s16)Script_GetKeywordValue(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->field9A = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->drawableKind ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);
}
static inline u8 *MonsterInit_Drawable(struct SceneActorRegistryResetView *actor)
{
    return actor->drawable;
}
static inline void MonsterInit_SetBehaviorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}
static inline void MonsterInit_SetCurrentPalette(struct SceneActorRegistryResetView *actor,
                                                 u32 palette)
{
    actor->alternateOffset = palette;
}
static inline void Tentacle_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                        u16 alternate)
{
    s32 r1v;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    MonsterInit_SetCurrentPalette(actor, MonsterInit_GetPaletteOffset(actor) + base);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette((void *)(actor->drawable + 0x20),
                                     SceneActorScript_GetManagerField62() - 0x5FFF);
}
static inline void MonsterInit_SetMotionControl(struct SceneActorRegistryResetView *actor,
                                                void *callback, u32 kind)
{
    actor->word104 = (u32)callback;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void MonsterInit_ClearMotion(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    MonsterInit_SetMotionControl(actor, 0, 0);
    actor->flags114 = 0;
}

static inline void MonsterInit_InstallPrimary(struct SceneActorRegistryResetView *actor, void *cb0,
                                              u32 (*cb1)(void *), void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *targetQuery,
                                              void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->contextCallback = cb0;
    MonsterInit_WriteAnimationQuery(&actor->animationQuery.word, cb1);
    actor->entryTarget224 = cb2;
    actor->entryTarget228 = cb3;
    actor->savedTarget = cb4;
    actor->reactionTarget230 = cb5;
    actor->deathControl = cb6;
    actor->field220 = hidden;
    actor->defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_086108B0);
    actor->callback248 = 0;
    actor->update26C = targetQuery;
    actor->field238 = SceneActorReaction_SinkAndRemove;
    actor->damageCallback = damage;
    actor->contactCallback = contact;
    actor->field240 = 0;
    MonsterInit_ClearMotion(actor);
}
static inline void Tentacle_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = TentacleEnemy_InitPlayerReference;
    actor->destroyCallback = TentacleBody_Destroy;
}

static inline void MonsterInit_SetCategory(struct SceneActorRegistryResetView *actor, u8 category)
{
    actor->category = category;
}
static inline void MonsterInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void Tentacle_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = ActorTrail_ResetAtSavedPosition;
    actor->field23C = SceneActor_ResetQueuedState;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, SceneActor_TryRequestFacingReaction);
    actor->field240 = (u32)sub_080AA744;
}

static inline void MonsterInit_InitializeReceiver(struct SceneActorRegistryResetView *actor)
{
    actor->field304 = 0;
}

static inline void MonsterInit_InitializeRegion(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_TestMotionFlags(actor, 2))
        actor->regionId = *(u32 *)(gGameState + 0x5A0);
    else
        actor->regionId = Region_FindContaining(&actor->body.position);
}

static inline void MonsterInit_SetTargetingFlags(struct SceneActorChaseTracking *targeting, u8 mask)
{
    mask |= targeting->flags;
    targeting->flags = mask;
}
static inline void Tentacle_EnableInitialFlags(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_SetTargetingFlags(&actor->tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 8);
    MonsterInit_SetMotionFlags(actor, 0x40);
}
s32 TentacleActor_Init(void *object)
{
    struct TentacleActor *tentacle = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    tentacle->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 9);
    TentacleActor_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD636, 0xC102);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        res = ScriptProc_Start(0xCB05, 0xD636);
        *(struct MonsterInit32Block *)heap = *(struct MonsterInit32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);

    if (Script_SeekToKeyword(0x70)) {
        s32 height;
        u16 *ground;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        actor->terrainCell = MonsterInit_CellId(&actor->body.position);
        height =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &actor->body.position);
        ground = &actor->groundHeight2BE;
        *ground = height;
        actor->field2EA = (u16)actor->body.position.y - height;
        actor->body.position.y = *ground;
        if (MonsterInit_GetDrawableKind(actor) == 0)
            *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
        else
            *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
        tentacle->savedPosition = actor->body.position;
    }

    if (Script_SeekToKeyword(0x50))
        tentacle->interval8E6 = Script_GetPc() ? Script_GetValue() : 300;
    TentacleActor_ResetRouteHistory(actor);
    Tentacle_ReadPalette(actor, 0x22A, 0x192);
    TentacleActor_InitCollision(actor);
    TentacleActor_InitSegmentUpdateState(actor);
    MonsterInit_InstallPrimary(actor, TentacleActor_OnUpdate, TentacleActor_OnResolveVariantValue,
                               TentacleBody_OnIdleState, TentacleBody_UpdateAttackWindup,
                               TentacleBody_ResetDirectionalPose,
                               TentacleBody_UpdateHiddenTransition, sub_080A71A0,
                               TentacleBody_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
                               SceneActorReaction_ResolveDamage, sub_080ABB18);
    Tentacle_InstallRoutes(actor);
    Tentacle_EnableInitialFlags(actor);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(actor, 0, 0, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetIdleControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_ClearColliderFlags((void *)((u8 *)actor + 0x134), ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_InitializeReceiver(actor);
    TentacleActor_SelectWanderRoute(actor);
    MonsterInit_SetState(actor, 0, 0, 2);
    MonsterInit_SetControl(actor, actor->entryTarget228, 1);
    SceneActorBehavior_InstallCallbackTables(actor);
    TentacleBody_InitBehaviorTable(actor);
    TentacleActor_InitStateHandlers(actor);
    TentacleBody_InitSegmentAnimations(actor, 0xD636, 0xC102, MonsterInit_GetDirection(actor));
    TentacleActor_ResetSegmentPositions(tentacle, &actor->body.position);
    TentacleActor_InitSegmentColliders(actor);
    TentacleActor_SelectWanderRoute(actor);
    TentacleActor_ActivateMarkedSegment(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Tentacle_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
