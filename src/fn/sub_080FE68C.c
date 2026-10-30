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
u8 sub_0807F81C(void *);
void SceneActorReaction_SinkAndRemove(void);
void sub_081363F4(void);
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
    struct SceneActorRegistryResetView base;
    u8 unknown6EC[0x75C];
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

static inline void AxeInit_InstallPrimary(struct SceneActorRegistryResetView *arg0, void *cb0,
                                          s32 (*cb1)(void), void *cb2, void *cb3, void *cb4,
                                          void *hidden, void *cb5, void *cb6, void *defaultReaction,
                                          s32 (*cb270)(void *), void *targetQuery, void *damage,
                                          void *contact)
{
    MonsterInit_SetBehaviorFlags(arg0, 0x2000);
    arg0->contextCallback = cb0;
    MonsterInit_WriteValueQuery(&arg0->animationQuery.value, cb1);
    arg0->entryTarget224 = cb2;
    arg0->entryTarget228 = cb3;
    arg0->savedTarget = cb4;
    arg0->reactionTarget230 = cb5;
    arg0->deathControl = cb6;
    arg0->field220 = hidden;
    arg0->defaultReaction = defaultReaction;
    MonsterInit_WriteResultCallback(&arg0->contactQuery.result, cb270);
    MonsterInit_WriteDataPointer(&arg0->commandTable.opaque, gUnk_086108B0);
    arg0->callback248 = 0;
    arg0->update26C = targetQuery;
    arg0->field238 = SceneActorReaction_SinkAndRemove;
    arg0->damageCallback = damage;
    arg0->contactCallback = contact;
    arg0->field240 = 0;
    MonsterInit_ClearMotion(arg0);
}

static inline void AxeInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Axe_OnRender;
    arg0->destroyCallback = Axe_ReleaseResources;
}

static inline void AxeInit_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = SceneActorControl_InitializeHoverState;
    actor->field23C = Axe_OnReaction;
}

static inline u8 AxeInit_BoundedVariant(s32 variant)
{
    if (variant > 1)
        return 1;
    return variant;
}

static inline void AxeInit_SetVariant(struct SceneActorRegistryResetView *actor, u8 variant)
{
    ((struct AxeActor *)actor)->variantED7 = variant;
}

static inline void AxeInit_InitializePaletteRange(struct SceneActorRegistryResetView *actor,
                                                  u32 alternate, u32 current)
{
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

s32 Axe_Init(void *object)
{
    struct AxeActor *arg0 = object;
    struct SceneActorRegistryResetView *actor = &arg0->base;
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
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x5291, 0x3F72);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5291, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(&arg0->base, &zero);
    arg0->halfHealthE48 = MonsterInit_GetHealth(actor) >> 1;
    Axe_InitAttackCooldowns(actor);
    if (Script_SeekToKeyword('p')) {
        u32 height;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        actor->terrainCell = MonsterInit_CellIndex(&actor->body.position);
        height =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &actor->body.position);
        actor->groundHeight2BE = height;
        arg0->groundHeightE58 = height;
    } else {
        arg0->groundHeightE58 = actor->groundHeight2BE;
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
                           sub_081363F4, SceneActorControl_AdvanceReactionCount);
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
        MonsterInit_MaskColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Axe_InitMotionTable(actor);
    Axe_InitActionCallbacks(actor);
    Axe_InitEventCallbacks(actor);
    MonsterInit_Drawable(actor)[7] = 1;
    actor->field2E8 = 0;
    SpriteGroup_InitAnimatedSprites(actor);
    Axe_InitRoutePointHistory(actor);
    ActorCollision_InitThreeAuxiliaryAttacks(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    actor->saved.privateFlags = arg0->savedFlagsE4C;
    AxeInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
