#include "scene_body_api.h"
#include "global.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"

void Octopus_InitProfileId(void *);
s32 Octopus_Update(void *);
void Octopus_UpdateDefeat(void *);
void SceneActorControl_BeginSoundTransition_080EBC38(void);
void SceneActorControl_UpdateMotionPhase_080EBD0C(void);
void SceneActorCollision_InitCompactBody(void *);
void Octopus_InitControlPresets(void *);
void Octopus_BeginIdleAnimation(void *);
void Octopus_UpdateInteraction(void);
void Octopus_Destroy(void);
extern const u8 gUnk_0860635C[];

void Memory_Clear(void *, u32);
u32 SceneActorReaction_OnEnemyDefault(void);
u8 SceneActorTarget_UpdateTrackedContact(void *);

void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
void SceneActorScript_OnCommand(void);
void SceneActorReaction_SetMotionMode(void);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorBehavior_InitializeShadow(void *);
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

struct OctopusInstanceState {
    u8 unknown0[0x624];
    u8 unknown624[8];
    u32 captureFlags;
    u8 unknown630[6];
    u16 field636;
    u8 unknown638;
    u8 field639;
    u8 field63A;
    u8 unknown63B;
};

extern u8 *gUnknown_02000580;

static inline void OctopusInit_DrawablePosition(struct ChaseActor *actor)
{
    if (actor->base.drawableKind == 0)
        *(struct ContactPosition *)(actor->base.drawable + 0x1C) = actor->base.body.position;
    else
        *(struct ContactPosition *)(actor->base.drawable + 0x40) = actor->base.body.position;
}

static inline void OctopusInit_InitializeBody(void *object, u32 *fill)
{
    struct ChaseActor *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, variant;
    u16 scriptParameter;
    struct SceneActorRegistryRoute *rt;
    arg0->base.flags2CC = 0;
    arg0->base.flags2D0 = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.flags2C0 = Script_GetKeywordValue('M', 0);
    arg0->base.field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue('N', 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword('l')) {
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
    if (Script_GetKeywordValue('b', 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    scriptParameter = (u16)Script_GetKeywordValue('F', 0);
    arg0->base.script1F0 = scriptParameter;
    scriptParameter = (u16)Script_GetKeywordValue('R', 0);
    arg0->base.field1EC = scriptParameter;

    if (Script_SeekToKeyword('A')) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.field9F = MonsterInit_ReadByte(0);
        arg0->base.fieldA0 = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword('X')) {
        arg0->base.fieldAC = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->base.fieldB2 = Script_GetKeywordValue('K', 2);
    variant = (s16)Script_GetKeywordValue('V', -1);
    if (variant < 0)
        variant = 0xFFFF;
    arg0->base.field9A = variant;
    if (Script_GetKeywordValue('H', 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue('L', 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->base.body.position, rt);
        OctopusInit_DrawablePosition(arg0);
        arg0->base.body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor(&group->common.placement, body,
                                     SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->base.drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x40, 0x40);

    group->common.bodyId = arg0->base.body.id;
    group->common.bodyPosition = &arg0->base.body.position;
}

static inline void OctopusInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.messageCallback = SceneActorScript_OnCommand;
    arg0->base.updateCallback = Octopus_UpdateInteraction;
    arg0->base.destroyCallback = Octopus_Destroy;
}

static inline void OctopusInit_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.callback248 = SceneActorControl_UpdateMotionPhase_080EBD0C;
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_0860635C);
}

static inline void OctopusInit_ResetCapture(void *object)
{
    struct ChaseActor *actor = object;
    ((struct OctopusInstanceState *)actor)->captureFlags = 0;
}

s32 Octopus_Init(void *object)
{
    struct ChaseActor *arg0 = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    OctopusInit_ResetCapture(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 14);
    Octopus_InitProfileId(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD291, 0xDCEB);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0xD291, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    OctopusInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        ((struct OctopusInstanceState *)arg0)->field636 = MonsterInit_ReadHalfword(0);
        ((struct OctopusInstanceState *)arg0)->field63A = MonsterInit_ReadByte(0);
        ((struct OctopusInstanceState *)arg0)->field639 = MonsterInit_ReadByte(0);
    }
    actor->base.body.position.x += 0x20;
    actor->base.body.position.z += 0x20;
    OctopusInit_DrawablePosition(actor);
    MonsterInit_ReadPalette(actor, 0x248, 0x192);
    SceneActorCollision_InitCompactBody(actor);
    MonsterInit_InstallSequenceCallbacks(actor, Octopus_Update, 0, 0, 0, 0, 0, 0,
                                         Octopus_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
                                         0, SceneActorReaction_ResolvePlacementDamage,
                                         SceneActorControl_BeginSoundTransition_080EBC38);
    MonsterInit_InitializeGroup(group, 0, 0, 0, 0, 0, 0, 1);
    OctopusInit_InstallRoutes(actor);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x408);
    MonsterInit_SetMotionFlags(actor, 0x40);
    if (Script_GetKeywordValue('E', 0))
        MonsterInit_SetStatusFlags(actor, 0x2000000);
    SceneActorTerrain_ReadScriptParameters(actor);
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
    Octopus_InitControlPresets(actor);
    Octopus_BeginIdleAnimation(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    OctopusInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
