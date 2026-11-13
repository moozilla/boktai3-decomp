#ifndef GUARD_ACTOR_ABILITY_H
#define GUARD_ACTOR_ABILITY_H
#include "engine.h"
#include "actor_stats.h"
#include "modifier_list.h"
enum ActorAbilitySourceIndex {
    ACTOR_ABILITY_BYTE_SOURCE = 5,
    ACTOR_ABILITY_FOURTH_SOURCE = 14,
    ACTOR_ABILITY_THIRD_SOURCE = 15
};
struct ActorAbilityOwner {
    u8 unknown0[0x18];
    struct ActorStats stats;
    u8 unknown608[8];
    struct CharacterPreviewModel model;
    u8 unknown864[0x80]; /* the model ends at 0x864 with its appearance bytes */
    u32 effectFlags;
    u32 projectile;
    u8 unknown8EC[0xC];
    s8 direction;
};
struct ActorAbilityProjectileParameters {
    u32 projectile, y, z, flags;
};
u8 Modifier_IsFourthSourceReady(struct ActorAbility *);
u32 CallbackStatus_ResetSelectorAndTest(struct ActorAbility *);
void CallbackList_SubtractStateTenValue(struct ActorStats *, struct ActorAbility *, s32);
s32 CallbackList_RunMatchingState(void *, s32, s32);
s32 CharacterModel_GetPrimaryPaletteIndex(void *);
void PalFade_Start(void *, s32, s32, s32, s32, s32, u32, s32);
void Character_SetLowResourceColorEffect(void *, void *);
s32 SceneProcSprites_CreateEntry(void *, s32, u32, s32, void *);
s32 SpritePool_StartEntry(void *, s32, s32, void *, s32);
s8 SpritePairPool_StartEntry(void *, void *, s32, void *, s32);
s32 ScriptRecord_FindAttributeModifier(void *, s32);
s32 ScriptRecord_FindModifier(void *, s32);
void ScriptRecord_FindByteModifier(void *, u32);
s32 ModifierAction_ActivateCost(struct ActorAbility *);
s32 ActorAbility_ActivatePaletteEffect(struct ActorAbility *);
s32 ActorAbility_UpdateRecovery(struct ActorAbility *);
s32 ActorAbility_SpawnProjectile(struct ActorAbility *);
s32 ActorAbility_SpawnAlternateProjectile(struct ActorAbility *);
s32 ActorAction_TrySpawn(struct ActorAbility *);
s32 Modifier_EvaluateThirdSourceModeOne(struct ActorSecondaryAbility *);
s32 Modifier_EvaluateThirdSourceModeTwo(struct ActorSecondaryAbility *);
#endif
