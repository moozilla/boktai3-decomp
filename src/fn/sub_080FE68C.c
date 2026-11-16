#include "global.h"
#include "scene_actor_init_group.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "player.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"

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

void Memory_Clear(void *, u32);
void SceneActorReaction_OnEnemyDefault(void);
u8 SceneActorTarget_UpdateTrackedContact(void *);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorReaction_ResolvePlacementDamage(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
void SceneActorScript_OnCommand(void);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);

void *Heap_Alloc(u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
extern const u8 gUnk_086108B0[];
void AnimData_SelectPaletteBank(void *, s32);
void Anim_Step(void *, void *);
u8 SceneActorRegistry_LinkActor(void *);

struct AxeActor {
    struct ChaseActor base;
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

extern u8 *gUnknown_02000580;

static inline void AxeInit_InstallPrimary(struct ChaseActor *arg0, void *cb0, s32 (*cb1)(void),
                                          void *cb2, void *cb3, void *cb4, void *hidden, void *cb5,
                                          void *cb6, void *defaultReaction, s32 (*cb270)(void *),
                                          void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(arg0, 0x2000);
    arg0->base.onUpdate = cb0;
    MonsterInit_WriteValueQuery(&arg0->base.animationQuery.value, cb1);
    arg0->base.onEntry = cb2;
    arg0->base.onAlternateEntry = cb3;
    arg0->base.onStanding = cb4;
    arg0->base.onReaction = cb5;
    arg0->base.onDefeat = cb6;
    arg0->base.onHidden = hidden;
    arg0->base.defaultReaction = defaultReaction;
    MonsterInit_WriteResultCallback(&arg0->base.contactQuery.result, cb270);
    MonsterInit_WriteDataPointer(&arg0->base.commandTable.opaque, gUnk_086108B0);
    arg0->base.callback248 = 0;
    arg0->base.targetQuery = targetQuery;
    arg0->base.field238 = SceneActorReaction_SinkAndRemove;
    arg0->base.onDamage = damage;
    arg0->base.onContact = contact;
    arg0->base.field240 = 0;
    MonsterInit_ClearMotion(arg0);
}

static inline void AxeInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = Axe_OnRender;
    arg0->base.destroy = Axe_ReleaseResources;
}

static inline void AxeInit_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.callback248 = SceneActorControl_InitializeHoverState;
    actor->base.field23C = Axe_OnReaction;
}

static inline u8 AxeInit_BoundedVariant(s32 variant)
{
    if (variant > 1)
        return 1;
    return variant;
}

static inline void AxeInit_SetVariant(struct ChaseActor *actor, u8 variant)
{
    ((struct AxeActor *)actor)->variantED7 = variant;
}

static inline void AxeInit_InitializePaletteRange(struct ChaseActor *actor, u32 alternate,
                                                  u32 current)
{
    actor->base.alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->base.paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

s32 Axe_Init(void *object)
{
    struct AxeActor *arg0 = object;
    struct ChaseActor *actor = &arg0->base;
    struct SceneActorChaseGroup *group = &arg0->base.group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 variant;

    arg0->savedFlagsE4C = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    ((struct AxeActor *)actor)->variantED7 = Script_GetKeywordValue('t', 0);
    variant = AxeInit_BoundedVariant(((struct AxeActor *)actor)->variantED7);
    AxeInit_SetVariant(&arg0->base, variant);
    if (variant == 0)
        MonsterInit_SetCategory(actor, 12);
    else
        MonsterInit_SetCategory(actor, 13);
    Axe_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x5291, 0x3F72);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5291, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(&arg0->base, &zero);
    arg0->halfHealthE48 = MonsterInit_GetHealth(actor) >> 1;
    Axe_InitAttackCooldowns(actor);
    if (Script_SeekToKeyword('p')) {
        u32 height;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        actor->base.terrainCell = MonsterInit_CellIndex(&actor->base.body.position);
        height = MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor),
                                           &actor->base.body.position);
        actor->base.groundHeight = height;
        arg0->groundHeightE58 = height;
    } else {
        arg0->groundHeightE58 = actor->base.groundHeight;
    }
    arg0->fieldEB6 = Script_GetKeywordValue('w', -1);
    ActorGroup_ReadScriptOptions(actor);
    arg0->playerPositionF1C = gPlayers[0]->position;
    MonsterInit_ReadPalette(actor, 0x2BD, 0x192);
    AxeInit_InitializePaletteRange(actor, 0, 2);
    Axe_InitEffectPalettes(actor);
    MonsterInit_SetAlternatePaletteOffset(actor, 0x1EE);
    Axe_InitColliders(actor);
    AxeInit_InstallPrimary(actor, Axe_OnUpdate, Axe_GetActorCategory, Axe_UpdateTurnAnimation,
                           Axe_OnAnimationReset, sub_080F93A4, 0, Axe_ConvergeRecoveryMotion,
                           Axe_UpdateDefeat, SceneActorReaction_OnEnemyDefault,
                           SceneActorControl_CanAdvanceHover, SceneActorTarget_ResolvePendingObject,
                           SceneActorReaction_ResolvePlacementDamage,
                           SceneActorControl_AdvanceReactionCount);
    MonsterInit_InitializeGroup(group, (u8 *)arg0 + 0xED8, (u8 *)arg0 + 0xEFC, 0, 0, 0, 0, 1);
    AxeInit_InstallRoutes(actor);
    group->responseCallback = 0;
    group->field5EC = 0;
    group->field614 = Axe_ReleaseAndIdle;
    group->field610 = Axe_UpdateDamageSpin;
    MonsterInit_SetMotionFlags(actor, 0x60);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x02100408);
    SceneActorBehavior_InitializeShadow(actor);
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
    Axe_InitMotionTable(actor);
    Axe_InitActionCallbacks(actor);
    Axe_InitEventCallbacks(actor);
    MonsterInit_Drawable(actor)[7] = 1;
    actor->base.field2E8 = 0;
    SpriteGroup_InitAnimatedSprites(actor);
    Axe_InitRoutePointHistory(actor);
    ActorCollision_InitThreeAuxiliaryAttacks(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    actor->base.saved.privateFlags = arg0->savedFlagsE4C;
    AxeInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
