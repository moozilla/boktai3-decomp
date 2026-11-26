#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/collision/scene_body_api.h"
#include "global.h"
#include "monsters/bat.h"
#include "scene/actor/scene_actor_state.h"
#include "engine/video/render.h"
#include "engine/engine_globals.h"
#include "engine/video/display_sequence_inline.h"
#include "monsters/scene_actor_init_group.h"
#include "monsters/scene_actor_init_graphics.h"

void *Heap_Alloc(u32);

void Memory_Clear(void *, u32);

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorControl_SetDefaultProfile(void *);

void Bat_OnUpdate(void);

void SceneActorControl_GetTravelProfile(void);

void SceneActorControl_UpdateFacingAnimation(void);

void SceneActorControl_ResetDirectionalAnimation(void);

void SceneActorControl_ConvergeSavedMotion(void);

void SceneActorControl_CompleteScriptedResponse(void);

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolveDamage(void);

void SceneActorPlacement_PlayResponseAndResetState(void);

void SceneActorReaction_SinkAndRemove(void);

void SceneActorControl_RestoreMotion(void);

void SceneActorControl_SetCollisionResponse(void);

void SceneActorControl_TryActivateProximityResponse(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void SceneActorControl_BindPrimaryPlayer(void);

void Bat_Destroy(void *);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorControl_InitContactColliders(void *);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(void *, s32);

void Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorControl_SetDefaultPreset(void *);

void SceneActorControl_SetApproachCallbacks(void *);

void SceneActorControl_SetStoredMotionCallbacks(void *);

void SceneActorControl_RestartPositionCurve(void *);

void ActorShadow_Disable(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_08605C74[];

extern SceneActorCommandHandler const gUnk_086108B0[];

static inline void MonsterInit_InitializeBody(struct ChaseActor *arg0)
{
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit;
    s32 scriptVariant;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    ZeroFill8(&origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->base.tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
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
    arg0->base.hudKind = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)MonsterInit_ReadKeywordHalfword(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->base.hudLabel = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->base.drawableKind ? 0 : arg0->base.drawable);
    SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x40, 0x40);
}

static inline void MonsterInit_ReadBatPalette(struct ChaseActor *actor, u16 base, u16 alternate)
{
    s32 r1v;
    actor->base.paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0x2D4);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->base.drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->base.drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette((void *)(actor->base.drawable + 0x20),
                                     SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct ChaseActor *actor, void *cb0, void *cb1,
                                              void *cb2, void *cb3, void *cb4, void *hidden,
                                              void *cb5, void *cb6, void *defaultReaction,
                                              void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->base.onUpdate = cb0;
    MonsterInit_WriteCallback(&actor->base.animationQuery.opaque, cb1);
    actor->base.onEntry = cb2;
    actor->base.onAlternateEntry = cb3;
    actor->base.onStanding = cb4;
    actor->base.onReaction = cb5;
    actor->base.onDefeat = cb6;
    actor->base.onHidden = hidden;
    actor->base.defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_086108B0);
    actor->base.routeReset = 0;
    actor->base.targetQuery = targetQuery;
    actor->base.sinkCallback = SceneActorReaction_SinkAndRemove;
    actor->base.onDamage = damage;
    actor->base.onContact = contact;
    actor->base.field240 = 0;
    MonsterInit_ClearMotion(actor);
}

static inline void Bat_InstallCompletionCallbacks(struct ChaseActor *actor)
{
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)SceneActorControl_BindPrimaryPlayer;
    actor->base.destroy = (SceneActorResultCallback)Bat_Destroy;
}

static inline void MonsterInit_InstallBatRoutes(struct ChaseActor *actor)
{
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_08605C74);
    actor->base.routeReset = SceneActorControl_RestoreMotion;
    actor->base.reactionGate = SceneActorControl_SetCollisionResponse;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque,
                              SceneActorControl_TryActivateProximityResponse);
}

s32 Bat_Init(void *object)
{
    struct Bat *bat = object;
    struct ChaseActor *actor = object;
    struct ContactPosition tmp;
    u32 flag;
    bat->flags3D4 = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_BAT);
    SceneActorControl_SetDefaultProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xDA6D, 0x5BB7);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        ZeroFill8(&tmp);
        res = ScriptProc_Start(0xCB05, 0xDA6D);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        AnimSprite_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor);
    if (Script_SeekToKeyword(0x67)) {
        bat->parameter3DA = MonsterInit_ReadByte(0);
        bat->parameter3DC = MonsterInit_ReadByte(0);
    } else {
        bat->parameter3DA = 0x3C;
        bat->parameter3DC = 3;
    }
    if (Script_GetKeywordValue(0x45, 0) == 0)
        actor->base.body.direction = 0;
    else
        actor->base.body.direction = 0x40;
    if (Script_SeekToKeyword(0x70)) {
        struct ContactPosition position;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        position = actor->base.body.position;
        if (MonsterInit_GetDirection(actor) == 0)
            position.x += 0x80;
        else
            position.z += 0x80;
        bat->initialPosition = position;
        actor->base.terrainCell = MonsterInit_CellId(&bat->initialPosition);
        {
            s32 groundHeight =
                MonsterInit_TerrainHeight(actor->base.terrainCell, &bat->initialPosition);
            s32 offset;
            u16 *ground = &actor->base.groundHeight;
            *ground = groundHeight;
            offset = (u16)actor->base.body.position.y - groundHeight;
            actor->base.heightOffset = offset;
            bat->height = offset;
            actor->base.body.position.y = *ground;
            bat->field3DE = *ground;
            bat->field3EA = *ground;
        }
    }
    MonsterInit_ReadBatPalette(actor, 0x2D2, 0x192);

    SceneActorControl_InitContactColliders(actor);
    MonsterInit_InstallPrimary(
        actor, Bat_OnUpdate, SceneActorControl_GetTravelProfile, 0,
        SceneActorControl_UpdateFacingAnimation, SceneActorControl_ResetDirectionalAnimation, 0,
        SceneActorControl_ConvergeSavedMotion, SceneActorControl_CompleteScriptedResponse,
        SceneActorReaction_OnEnemyDefault, 0, SceneActorReaction_ResolveDamage,
        SceneActorPlacement_PlayResponseAndResetState);
    MonsterInit_InstallBatRoutes(actor);
    MonsterInit_SetStatusFlags(actor, 0x02100408);
    MonsterInit_SetMotionFlags(actor, 0x60);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        SceneActorTerrain_EnterState(actor, 0, 0, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    SceneActorControl_SetDefaultPreset(actor);
    SceneActorControl_SetApproachCallbacks(actor);
    SceneActorControl_SetStoredMotionCallbacks(actor);
    actor->base.field2E8 = 2;
    MonsterInit_Drawable(actor)[7] = 1;
    if (Script_GetKeywordValue(0x54, 0) == 0) {
        SceneActorControl_RestartPositionCurve(actor);
    } else {
        bat->flags3D4 |= 2;
        ActorShadow_Disable(actor->base.shadow);
    }
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Bat_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

void Heap_Free(void *);

s32 Bat_Init(void *);

void Bat_Create(void)
{
    void *m = Heap_Alloc(sizeof(struct Bat));
    if (m != 0) {
        Memory_Clear(m, sizeof(struct Bat));
        if (Bat_Init(m) < 0) {
            Bat_Destroy(m);
            Heap_Free(m);
        }
    }
}
