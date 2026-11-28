#include "scene/actor/actor_ability.h"

u8 Modifier_IsFourthSourceReady(struct ActorAbility *ability)
{
    if (ability->enabled && ability->remaining == 0)
        return TRUE;
    return FALSE;
}

void CallbackList_SubtractStateTenValue(struct ActorStats *stats, struct ActorAbility *ability,
                                        s32 amount)
{
    if (amount <= 0) {
        amount = ability->cost;
        amount = CallbackList_RunMatchingState(stats, amount, 10);
    }
    stats->value -= amount;
    if (stats->value < 0)
        stats->value = 0;
}

u32 CallbackStatus_ResetSelectorAndTest(struct ActorAbility *ability)
{
    ability->request = 0xff;
    if (!Modifier_IsFourthSourceReady(ability))
        return 0;
}

s32 ModifierAction_ActivateCost(struct ActorAbility *ability)
{
    struct ActorAbilityOwner *owner;
    struct ActorStats *stats;
    s32 cost;
    ability->request = 0xff;
    if (Modifier_IsFourthSourceReady(ability)) {
        owner = ability->owner;
        stats = &owner->actors[0].stats;
        if (!ability->cooldown) {
            cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
            if (stats->value >= cost) {
                CallbackList_SubtractStateTenValue(stats, ability, cost);
                ability->cooldown = ability->interval;
                ability->remaining = ability->duration;
                Sound_PlaySong(0x21e);
            }
        }
    }
    return 0;
}

s32 ActorAbility_ActivatePaletteEffect(struct ActorAbility *ability)
{
    struct ActorAbilityOwner *owner;
    struct ActorStats *stats;
    s32 cost;
    ability->request = 255;
    if (Modifier_IsFourthSourceReady(ability)) {
        owner = ability->owner;
        stats = &owner->actors[0].stats;
        if (!ability->cooldown) {
            cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
            if (stats->value >= cost) {
                s32 palette;
                ability->cooldown = ability->interval;
                CallbackList_SubtractStateTenValue(stats, ability, cost);
                palette = CharacterModel_GetPrimaryPaletteIndex(&owner->actors[0].model);
                PalFade_Start(stats, 0x385, 0x385, palette, stats->recoveryValue, 16, 4, 0);
                ability->remaining = ability->duration;
                Sound_PlaySong(0x3e3);
            }
        }
    }
    return 0;
}

s32 ActorAbility_UpdateRecovery(struct ActorAbility *ability)
{
    struct ActorAbilityOwner *owner;
    struct ActorStats *stats;
    s32 cost, palette;
    ability->request = 255;
    if (!Modifier_IsFourthSourceReady(ability))
        return 0;
    owner = ability->owner;
    stats = &owner->actors[0].stats;
    if (stats->amount >= stats->amountLimit)
        return 0;
    if (!ability->started && stats->value <= 15)
        return 0;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        return 0;
    CallbackList_SubtractStateTenValue(stats, ability, cost);
    stats->amount++;
    if (stats->amount > stats->amountLimit)
        stats->amount = stats->amountLimit;
    palette = CharacterModel_GetPrimaryPaletteIndex(&owner->actors[0].model);
    if (!ability->started) {
        PalFade_Start(stats, 0x386, 0x386, palette, stats->recoveryValue, 15, 4, 0);
        ability->started = 1;
    } else if (!(owner->actors[0].control.effectFlags & 1)) {
        PalFade_Start(stats, 0x386, 0x386, palette, stats->recoveryValue, 15, 4, 1);
    }
    if (ability->soundTimer == 0)
        ability->soundTimer = 30;
    else {
        ability->soundTimer--;
        if (ability->soundTimer == 10)
            Sound_PlaySong(0x95);
    }
    Character_SetLowResourceColorEffect(stats->state44, stats->state48);
    return 1;
}

s32 ActorAbility_SpawnProjectile(struct ActorAbility *ability)
{
    struct ActorAbilityOwner *owner;
    struct ActorStats *stats;
    s32 height, cost;
    /* Projectile launch snapshot. */
    struct ActorAbilityProjectileParameters args;
    ability->request = 255;
    if (!Modifier_IsFourthSourceReady(ability))
        return 0;
    owner = ability->owner;
    stats = &owner->actors[0].stats;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        return 0;
    args.projectile = owner->actors[0].control.projectile;
    args.y = 0;
    args.z = 0;
    height = stats->height - 0x12000;
    if (SceneProcSprites_CreateEntry(&stats->position, height, owner->actors[0].control.projectile,
                                     owner->actors[0].control.direction, &ability->id) >= 0) {
        CallbackList_SubtractStateTenValue(stats, ability, cost);
        ability->remaining = ability->duration;
        return 1;
    }
    return 0;
}

s32 ActorAbility_SpawnAlternateProjectile(struct ActorAbility *ability)
{
    struct ActorAbilityOwner *owner;
    struct ActorStats *stats;
    s32 height, cost;
    /* Projectile launch snapshot. */
    struct ActorAbilityProjectileParameters args;
    ability->request = 255;
    if (!Modifier_IsFourthSourceReady(ability))
        return 0;
    owner = ability->owner;
    stats = &owner->actors[0].stats;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        return 0;
    args.projectile = owner->actors[0].control.projectile;
    args.y = 0;
    args.z = 0;
    height = stats->height - 0x1a000;
    if (SpritePool_StartEntry(&stats->position, height, 0, &ability->id,
                              owner->actors[0].control.direction) >= 0) {
        CallbackList_SubtractStateTenValue(stats, ability, cost);
        ability->remaining = ability->duration;
        return 1;
    }
    return 0;
}

s32 ActorAction_TrySpawn(struct ActorAbility *ability)
{
    struct ActorAbilityOwner *owner;
    struct ActorStats *stats;
    s32 cost;
    struct ActorAbilityProjectileParameters args;
    if (!Modifier_IsFourthSourceReady(ability))
        return 0;
    owner = ability->owner;
    stats = &owner->actors[0].stats;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        return 0;
    args.projectile = owner->actors[0].control.projectile;
    args.y = 0;
    args.z = 0;
    ability->request = SpritePairPool_StartEntry(&stats->position, &stats->height, 0, &ability->id,
                                                 owner->actors[0].control.direction);
    if ((s8)ability->request >= 0) {
        CallbackList_SubtractStateTenValue(stats, ability, cost);
        ability->remaining = ability->duration;
        return 1;
    }
    return 0;
}

void Modifier_InitFourthSourceEffect(struct ActorStats *stats, u32 owner)
{
    u8 *selection = stats->state48;
    struct ActorAbility *ability = &stats->ability;
    ability->callback = NULL;
    ability->enabled = 0;
    ability->owner = (void *)owner;
    Memory_Clear(&ability->id, 0xc);
    if (ScriptRecord_FindAttributeModifier(&ability->id,
                                           (s8)selection[ACTOR_ABILITY_FOURTH_SOURCE]) < 0) {
        ability->callback = (void (*)(void))CallbackStatus_ResetSelectorAndTest;
        ability->enabled = 0;
    } else {
        switch (ability->kind) {
            case 0:
                ability->callback = (void (*)(void))ActorAbility_SpawnProjectile;
                ability->enabled = 1;
                break;
            case 1:
                ability->callback = (void (*)(void))ActorAbility_ActivatePaletteEffect;
                ability->enabled = 1;
                break;
            case 2:
                ability->callback = (void (*)(void))ActorAbility_SpawnAlternateProjectile;
                ability->enabled = 1;
                break;
            case 3:
                ability->callback = (void (*)(void))ActorAction_TrySpawn;
                ability->enabled = 1;
                break;
            case 4:
                ability->callback = (void (*)(void))ModifierAction_ActivateCost;
                ability->enabled = 1;
                break;
            case 5:
                ability->callback = (void (*)(void))ActorAbility_UpdateRecovery;
                ability->enabled = 1;
                break;
        }
    }
}

s32 ModifierAction_SpawnBurst(struct ActorStats *stats, void *target)
{
    struct ActorAbility *ability = &stats->ability;
    ability->request = 0xff;
    if (Modifier_IsFourthSourceReady(ability)) {
        if (!ability->cooldown) {
            ability->cooldown = ability->interval;
            CallbackList_SubtractStateTenValue(stats, ability, 0);
            PalFade_Start(stats, 0x385, 0x385, (s32)target, stats->recoveryValue, 16, 4, 0);
            ability->remaining = ability->duration;
            Sound_PlaySong(0x3e3);
        }
    }
    return 0;
}

static inline void ClearDamageFlag(u8 *flags, s32 mask)
{
    *flags &= mask;
}

s32 ActorAction_TickRecovery(struct ActorStats *stats, void *context)
{
    if (stats->ability.kind == 1 && stats->ability.cooldown) {
        stats->ability.cooldown--;
        ClearDamageFlag(&stats->damageFlags, ~1);
        stats->damageTimer = 0;
        stats->pendingDamage = 0;
        if (!stats->ability.cooldown)
            PalFade_Start(stats, (s32)context, stats->recoveryValue, 0, 0, 16, 1, 1);
    }
    if (stats->ability.remaining)
        stats->ability.remaining--;
}

s32 Modifier_EvaluateThirdSourceModeOne(struct ActorSecondaryAbility *ability)
{
    if (!ability->enabled)
        return -1;
    else
        return 1;
}

s32 Modifier_EvaluateThirdSourceModeTwo(struct ActorSecondaryAbility *ability)
{
    if (!ability->enabled)
        return -1;
    else
        return 1;
}

void ModifierAction_InitializeSecondary(struct ActorStats *stats, u32 argument)
{
    u8 *selection = stats->state48;
    struct ActorSecondaryAbility *ability = (void *)&stats->secondaryCallback;
    u16 *config;
    ability->callback = NULL;
    ability->enabled = 0;
    ability->argument = argument;
    config = ability->config;
    Memory_Clear(config, 12);
    ScriptRecord_FindModifier(config, selection[ACTOR_ABILITY_THIRD_SOURCE]);
    switch (ability->kind) {
        case 1:
            ability->callback = (void (*)(void))Modifier_EvaluateThirdSourceModeOne;
            ability->enabled = 1;
            break;
        case 2:
            ability->callback = (void (*)(void))Modifier_EvaluateThirdSourceModeTwo;
            ability->enabled = 1;
            break;
    }
}

void Modifier_InitSourceInfo(struct ActorStats *stats, u32 argument)
{
    u8 *selection = stats->state48;
    struct ActorAbilitySourceInfo *info = (void *)&stats->sourceInfoId;
    info->argument = argument;
    Memory_Clear(info, 8);
    ScriptRecord_FindByteModifier(info, selection[ACTOR_ABILITY_BYTE_SOURCE]);
}
