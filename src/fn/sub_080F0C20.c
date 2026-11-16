#include "scene_body_api.h"
#include "scene_actor_init_dispatch.h"
#include "global.h"
#include "ivy.h"
#include "scene_actor_init_group.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"
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
u32 Ivy_Destroy(struct SDestroy *s);
extern const u8 gUnk_08606400[];

void Memory_Clear(void *, u32);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorScript_OnCommand(void *actor, const u16 *command);
void SceneActorReaction_SetMotionMode(void);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);

void *Heap_Alloc(u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *, s32);
void Anim_Step(void *, void *);
u8 SceneActorRegistry_LinkActor(void *);

static inline void IvyInit_ResetState(struct ChaseActor *actor, u32 behavior)
{
    MonsterInit_SetState(actor, 0, 0, behavior);
}

static inline void IvyInit_InitializeBody(void *object, u32 *fill)
{
    struct ChaseActor *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit, gateDuration, statValue;
    u16 scriptParameter;
    struct SceneActorRegistryRoute *rt;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = Script_GetKeywordValue(0x4D, 0);
    arg0->base.field2C2 = 0x40;
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
    MonsterInit_ReadTargeting(&arg0->base.tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    scriptParameter = (u16)Script_GetKeywordValue(0x46, 0);
    arg0->base.script1F0 = scriptParameter;
    scriptParameter = (u16)Script_GetKeywordValue(0x52, 0);
    arg0->base.scriptId = scriptParameter;

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->base.hudKind = Script_GetKeywordValue(0x4B, 2);
    statValue = (s16)Script_GetKeywordValue(0x56, -1);
    if (statValue < 0)
        statValue = 0xFFFF;
    arg0->base.hudLabel = statValue;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->base.body.position, rt);
        MonsterInit_DrawablePosition(arg0);
        arg0->base.body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->base.drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->base.body.id;
    group->common.bodyPosition = &arg0->base.body.position;
}

static inline void IvyInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = (SceneActorResultCallback)Ivy_UpdateInteraction;
    arg0->base.destroy = (SceneActorResultCallback)Ivy_Destroy;
}

static inline void IvyInit_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.routeReset = Ivy_TickDrawable;
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_08606400);
}

static inline void IvyInit_ResetCapture(void *object)
{
    struct ChaseActor *actor = object;
    ((struct Ivy *)actor)->captureFlags = 0;
}

static inline void IvyInit_EnableColliderContact(struct ContactCollider *collider)
{
    u32 flags = collider->flags;
    u32 mask = 8;
    mask |= flags;
    mask &= ~4;
    collider->flags = mask;
}

static inline void IvyInit_ClearMotionFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.placementFlags = mask & actor->base.placementFlags;
}

s32 Ivy_Init(void *object)
{
    struct ChaseActor *arg0 = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    IvyInit_ResetCapture(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 24);
    Ivy_InitProfileId(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD638, 0x99DD);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0xD638, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    IvyInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        ((struct Ivy *)arg0)->field636 = MonsterInit_ReadHalfword(0);
        ((struct Ivy *)arg0)->interval = MonsterInit_ReadByte(0);
        ((struct Ivy *)arg0)->holdTicks = MonsterInit_ReadByte(0);
    }
    ((struct Ivy *)arg0)->field638 = Script_GetKeywordValue('d', 0);
    actor->base.body.position.x += 0x20;
    actor->base.body.position.z += 0x20;
    MonsterInit_DrawablePosition(actor);
    MonsterInit_ReadPalette(actor, 0x12A, 0x192);
    SceneActor_InitCollision5005(actor);
    MonsterInit_InstallOpaqueContactCallbacks(
        actor, Ivy_Update, 0, 0, 0, 0, 0, 0, Ivy_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
        0, SceneActorReaction_ResolvePlacementDamage, SceneActorControl_BeginSoundTransition);
    MonsterInit_InitializeGroup(group, 0, 0, 0, 0, 0, 0, 1);
    IvyInit_InstallRoutes(actor);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x408);
    MonsterInit_SetMotionFlags(actor, 0xC0);
    if (Script_GetKeywordValue('E', 0))
        MonsterInit_SetStatusFlags(actor, 0x2000000);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        IvyInit_ResetState(actor, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Ivy_InitControlPresets(actor);
    SceneActor_BeginReaction22(actor);
    if (MonsterInit_TestWorldFlags(0x400)) {
        IvyInit_ResetState(actor, 1);
        MonsterInit_SetControl(actor, SceneActorReaction_PlayHiddenCue, 0x2A);
        IvyInit_EnableColliderContact(&actor->base.colliderA);
        IvyInit_ClearMotionFlags(actor, ~0x800);
    }
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    IvyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
