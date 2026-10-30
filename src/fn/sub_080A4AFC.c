#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_group.h"

struct MonsterInit32Block {
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
void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *, s32);
u32 SceneActorReaction_OnEnemyDefault(void);
void sub_081363F4(void);
void SceneActorScript_OnCommand(void);
extern SceneActorCommandHandler const gUnk_086108B0[];
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
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

struct SpiderDeferredControl {
    u8 unknown0[8];
    u8 state, phase, mode, unknownB;
    u32 counter;
    void *callback;
};
struct SpiderInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[8];
    u16 flags62C;
    u16 timer62E;
    u8 bounds630[0x24];
    u8 bounds654[0x24];
};

void Spider_InitProfile(void *);
u32 Spider_OnUpdate(void *);
u32 Spider_GetAnimationEvent(void *);
void sub_080A21EC(void *);
void Spider_SelectActiveAnimation(void *);
void Spider_SelectIdleAnimation(void *);
void Spider_UpdateLandingMotion(void *);
void Spider_OnActionEvent(void *);
void Spider_InitColliders(void *);
void Spider_SetBehaviorTables(void *);
void SceneActorPlacement_BeginDefaultResponse(void);
u8 SceneActorPlacement_BeginContactRoute(void *);
u8 SceneActorPlacement_BeginNearbyPlayerContact(void *);
u8 SceneActorPlacement_SelectPlayerApproach(void *);
void SceneActorReaction_ResumeEntryTarget(void);
void SceneActorRoute_ResetStep(void);
void Spider_OnSecondaryColliderContact(void);
void Spider_UpdateHiddenRecovery(void);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

extern u8 *gUnknown_02000580;

void SceneActorMovement_ResolveRoutePosition(void *);
void Spider_UpdateDefeat(void *);
void SceneActorReaction_BeginTargetRecovery(void);
void Spider_OnSecondaryUpdate(void);
void Spider_Destroy(void);
extern const u8 gUnk_08605F34[];

static inline void MonsterInit_DrawablePosition(struct SceneActorRegistryResetView *actor)
{
    if (MonsterInit_GetDrawableKind(actor) == 0)
        *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
    else
        *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
}

static inline void MonsterInit_InitializeBody(void *object, u32 *fill)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = Script_GetKeywordValue(0x4D, 0);
    arg0->field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
        gateDuration = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    MonsterInit_InitializeGate(&group->common.gate, gateDuration);
    MonsterInit_ReadRange(&group->common.control);
    MonsterInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = Script_GetKeywordValue(0x4B, 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->field9A = identity;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->body.position, rt);
        MonsterInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, sub_08080BE0);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

struct MonsterInitWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
};
static inline void MonsterInit_SetWidgetScriptPalette(struct MonsterInitWidget *widget, s32 palette)
{
    widget->paletteBank = palette - 0x5FFF;
}

static inline void MonsterInit_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                           u32 alternate)
{
    s32 palette;
    actor->paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MonsterInit_GetPaletteOffset(actor) + base;
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, palette);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(actor->drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

static inline void MonsterInit_InstallPrimary(struct SceneActorRegistryResetView *arg0, void *cb0,
                                              u32 (*cb1)(void *), void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, u8 (*cb270)(void *),
                                              void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(arg0, 0x2000);
    arg0->contextCallback = cb0;
    MonsterInit_WriteAnimationQuery(&arg0->animationQuery.word, cb1);
    arg0->entryTarget224 = cb2;
    arg0->entryTarget228 = cb3;
    arg0->savedTarget = cb4;
    arg0->reactionTarget230 = cb5;
    arg0->deathControl = cb6;
    arg0->field220 = hidden;
    arg0->defaultReaction = defaultReaction;
    MonsterInit_WriteContactPredicate(&arg0->contactQuery.predicate, cb270);
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086108B0);
    arg0->callback248 = 0;
    arg0->update26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->damageCallback = damage;
    arg0->contactCallback = contact;
    arg0->field240 = 0;
    MonsterInit_ClearMotion(arg0);
}

static inline void MonsterInit_SetRouteReset(struct SceneActorRegistryResetView *actor,
                                             void *callback)
{
    actor->callback248 = callback;
}
static inline void MonsterInit_SetReactionGate(struct SceneActorRegistryResetView *actor,
                                               void *callback)
{
    actor->field23C = callback;
}

static inline void MonsterInit_EnableInitialFlags(struct SceneActorRegistryResetView *arg0)
{
    MonsterInit_SetTargetingFlags(&arg0->tracking, 1);
    MonsterInit_SetBehaviorFlags(arg0, 0x400000);
    MonsterInit_SetStatusFlags(arg0, 8);
}

static inline void MonsterInit_ReadInitialMode(struct SceneActorRegistryResetView *arg0,
                                               struct SceneActorChaseGroup *group)
{
    s32 i;
    i = Script_GetKeywordValue(0x75, 0);
    arg0->group.effect = 0x21;
    if (i != 0) {
        MonsterInit_SetBehaviorFlags(arg0, 0x40000);
        if (i == 2)
            MonsterInit_SetBehaviorFlags(arg0, 0x80000);
    }
}

static inline void MonsterInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Spider_OnSecondaryUpdate;
    arg0->destroyCallback = Spider_Destroy;
}

static inline void MonsterInit_InitializeSprite(u8 *drawable, u16 resource, u16 sequence)
{
    AnimData_Load(drawable + 0x2C, resource);
    Sprite_Init(drawable, drawable + 0x2C, 0);
    SceneActorAnimation_InitEnemySequence(drawable, sequence);
}

static inline void Spider_SetFlags(struct SpiderInstanceState *actor, u32 mask)
{
    actor->flags62C |= mask;
}
static inline void Spider_InitializeDeferred(struct SpiderDeferredControl *control, void *callback,
                                             u32 mode, u32 timer)
{
    control->counter = timer;
    control->callback = callback;
    control->state = 0;
    control->phase = 0;
    control->mode = mode;
}

static inline void MonsterInit_InitializeWidget(u8 *drawable, u16 script, u16 resource,
                                                struct ContactPosition *position)
{
    void *source = ScriptProc_Start(script, resource);
    *(struct MonsterInit32Block *)drawable = *(struct MonsterInit32Block *)source;
    ResourceHeader_Relocate(drawable, source);
    Widget_InitLinked(drawable + 0x20, drawable, 0, 0, 2, 2, 0x3C, position);
}
static inline void Spider_InstallContact(struct SceneActorRegistryResetView *actor,
                                         struct SceneActorChaseGroup *group)
{
    MonsterInit_WriteDataPointer(&actor->commandTable.opaque, gUnk_08605F34);
    group->common.gate.control = SceneActorReaction_ResumeEntryTarget;
    if (MonsterInit_GetPaletteOffset(actor) == 0)
        MonsterInit_WriteContactPredicate(&actor->contactQuery.predicate,
                                          SceneActorPlacement_BeginNearbyPlayerContact);
    else
        MonsterInit_WriteContactPredicate(&actor->contactQuery.predicate,
                                          SceneActorPlacement_SelectPlayerApproach);
}
s32 Spider_Init(void *object)
{
    struct SpiderInstanceState *arg0 = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &((struct SceneActorRegistryResetView *)arg0)->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 extra;

    arg0->flags62C = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 2);
    Spider_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x528F, 0xB6EE);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x528F, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    MonsterInit_ReadPalette(actor, 0xC8, 0x192);
    Spider_InitColliders(actor);
    extra = Script_GetKeywordValue(0x66, 0);
    if (Script_GetKeywordValue(0x54, 0))
        Spider_SetFlags(arg0, 0x10);
    MonsterInit_InstallPrimary(actor, Spider_OnUpdate, Spider_GetAnimationEvent, sub_080A21EC,
                               Spider_SelectActiveAnimation, Spider_SelectIdleAnimation,
                               Spider_OnActionEvent, Spider_UpdateLandingMotion,
                               Spider_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
                               SceneActorPlacement_BeginContactRoute, sub_081363F4,
                               SceneActorPlacement_BeginDefaultResponse);
    MonsterInit_InitializeGroupWithBounds(
        group, arg0->bounds630, arg0->bounds654, Spider_OnSecondaryColliderContact, 0,
        SceneActorReaction_BeginTargetRecovery, 0, 1, &actor->body);
    SceneActorMovement_ResolveRoutePosition(actor);
    MonsterInit_SetRouteReset(actor, SceneActorRoute_ResetStep);
    MonsterInit_SetReactionGate(actor, 0);
    Spider_InstallContact(actor, group);
    MonsterInit_EnableInitialFlags(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(actor, group);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(actor, 0, 0, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_ClearColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Spider_SetBehaviorTables(actor);
    if (extra != 0) {
        actor->field2EA = extra;
        actor->field2E8 = 2;
        MonsterInit_SetControl(actor, Spider_UpdateHiddenRecovery, 0x23);
        MonsterInit_SetState(actor, 3, 0, 0);
        ScaledShadow_Initialize((u8 *)actor + 0x360, &actor->body.position, 0x40, 0x10, 0x60, 0x64,
                                0xA00, 1, 5, 1, 0);
        Spider_SetFlags(arg0, 1);
    } else {
        MonsterInit_SetControl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 0xE);
        Spider_InitializeDeferred((void *)&group->saved, actor->entryTarget228, 7, 0x3C);
    }
    arg0->timer62E = 0x12C;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    MonsterInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
