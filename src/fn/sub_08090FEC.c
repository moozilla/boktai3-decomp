#include "scene_body_api.h"
#include "global.h"
#include "scene_actor_state.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorScript_OnCommand(void);
void AnimData_SelectPaletteBank(void *, s32);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *, void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
u8 SceneActorRegistry_LinkActor(void *);

void Slime_InitProfile(void *);
void Slime_InitColliders(void *);
void Slime_SetPrimaryStateTable(void *);
void Slime_SetSecondaryStateTable(void *);
void Bat_ReadScriptValuePair(void *);
void Slime_Update(void);
void Slime_GetAnimationMovePulse(void);
void Slime_ResetAnimation(void);
void Slime_RollMoveFlag(void);
void Slime_ConvergeReactionMotion(void);
void Bat_RunDefeatScript(void);
void Slime_ResolveDamageReaction(void);
void Slime_ResetIdlePlayback(void);
void Slime_FollowPlayer(void);
void Slime_TryEnrage(void);
void Slime_PlayPendingSound(void);
void Slime_OnMessage(void);
void Slime_Destroy(void);

extern SceneActorCommandHandler const gUnk_086108B0[];
struct SlimeActor {
    u8 unknown0[0x3D4];
    u16 unknown3D4;
    u16 timeout3D6;
    u16 ground3D8;
    u16 halfHealth3DA;
    u8 unknown3DC[0x2E];
    u16 flags40A;
};

static inline void MonsterInit_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit;
    s32 scriptVariant;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
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
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)MonsterInit_ReadKeywordHalfword(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->field9A = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);
}
static inline void Slime_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                     u16 alternate)
{
    s32 r1v;
    actor->paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0xCC);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette((void *)(actor->drawable + 0x20),
                                     SceneActorScript_GetManagerField62() - 0x5FFF);
}
static inline void MonsterInit_InstallPrimary(struct SceneActorRegistryResetView *actor, void *cb0,
                                              void *cb1, void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *targetQuery,
                                              void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->contextCallback = cb0;
    MonsterInit_WriteCallback(&actor->animationQuery.opaque, cb1);
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
static inline void Slime_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = Slime_OnMessage;
    actor->destroyCallback = Slime_Destroy;
}

static inline void Slime_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = Slime_ResetIdlePlayback;
    actor->field23C = Slime_FollowPlayer;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, Slime_TryEnrage);
    actor->field240 = Slime_PlayPendingSound;
}
static inline u8 Slime_TestFlags(struct SlimeActor *slime, u32 mask)
{
    if (slime->flags40A & mask)
        return 1;
    return 0;
}
static inline void Slime_SetFlags(struct SlimeActor *slime, u16 mask)
{
    slime->flags40A |= mask;
}

static inline s32 MonsterInit_UpdateTerrainCell(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition *position = &actor->body.position;
    actor->terrainCell = MonsterInit_CellId(position);
    return MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), position);
}
s32 Slime_Init(void *object)
{
    struct SlimeActor *slime = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 j;
    slime->flags40A = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x16);
    Slime_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xB96C, 0xA929);
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
        res = ScriptProc_Start(0xCB05, 0xB96C);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    slime->halfHealth3DA = MonsterInit_GetHealth(actor) >> 1;
    if (Script_SeekToKeyword('p')) {
        s32 ground;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        ground = MonsterInit_UpdateTerrainCell(actor);
        actor->groundHeight2BE = ground;
        slime->ground3D8 = ground;
        if (MonsterInit_GetDrawableKind(actor) == 0)
            *(struct ContactPosition *)(actor->drawable + 0x1C) = actor->body.position;
        else
            *(struct ContactPosition *)(actor->drawable + 0x40) = actor->body.position;
    }
    j = Script_SeekToKeyword('j');
    if (j) {
        actor->secondaryMotion.x = MonsterInit_ReadHalfword(0);
        actor->secondaryMotion.y = MonsterInit_ReadHalfword(0);
        actor->secondaryMotion.z = MonsterInit_ReadHalfword(0);
        Slime_SetFlags(slime, 0x18);
        {
            s32 health = Script_GetKeywordValue('m', 0);
            if (health)
                actor->defaultHealth = health;
        }
        actor->field2E8 = 1;
    } else {
        actor->field2E8 = j;
    }
    actor->colliderA.responseValue = MonsterInit_ReadKeywordHalfword('n', 0);
    slime->timeout3D6 = 600;
    actor->field2EA = Script_GetKeywordValue('f', 0);
    if (actor->field2EA)
        Slime_SetFlags(slime, 0x10);
    Bat_ReadScriptValuePair(actor);
    Slime_ReadPalette(actor, 0xCC, 0x192);
    Slime_InitColliders(actor);
    MonsterInit_InstallPrimary(actor, Slime_Update, Slime_GetAnimationMovePulse, 0,
                               Slime_ResetAnimation, Slime_RollMoveFlag, 0,
                               Slime_ConvergeReactionMotion, Bat_RunDefeatScript,
                               SceneActorReaction_OnEnemyDefault, 0,
                               SceneActorReaction_ResolveDamage, Slime_ResolveDamageReaction);
    Slime_InstallRoutes(actor);
    MonsterInit_SetStatusFlags(actor, 8);
    MonsterInit_SetMotionFlags(actor, 0x40);
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
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(actor->drawable + 0x20, actor->drawable);
    MonsterInit_ClearReceiver(actor);
    if (Slime_TestFlags(slime, 8)) {
        MonsterInit_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        MonsterInit_SetState(actor, 0, 0, 2);
        MonsterInit_SetStatusFlags(actor, 0x800);
    }
    SceneActorBehavior_InstallCallbackTables(actor);
    Slime_SetPrimaryStateTable(actor);
    Slime_SetSecondaryStateTable(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Slime_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}
