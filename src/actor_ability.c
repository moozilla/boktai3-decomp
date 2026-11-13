#include "actor_ability.h"

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
    u32 result;
    ability->request = 0xff;
    result = Modifier_IsFourthSourceReady(ability);
    if ((u8)result == 0)
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
        stats = &owner->stats;
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
    u32 zero;
    s32 cost;
    ability->request = 255;
    if (Modifier_IsFourthSourceReady(ability)) {
        owner = ability->owner;
        stats = &owner->stats;
        zero = ability->cooldown;
        if (zero == 0) {
            cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
            if (stats->value >= cost) {
                s32 palette;
                ability->cooldown = ability->interval;
                CallbackList_SubtractStateTenValue(stats, ability, cost);
                palette = CharacterModel_GetPrimaryPaletteIndex(&owner->model);
                PalFade_Start(stats, 0x385, 0x385, palette, stats->recoveryValue, 16, 4, zero);
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
    u32 started;
    ability->request = 255;
    if (!Modifier_IsFourthSourceReady(ability))
        return 0;
    owner = ability->owner;
    stats = &owner->stats;
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
    palette = CharacterModel_GetPrimaryPaletteIndex(&owner->model);
    started = ability->started;
    if (started == 0) {
        PalFade_Start(stats, 0x386, 0x386, palette, stats->recoveryValue, 15, 4, started);
        ability->started = 1;
    } else if (!(owner->effectFlags & 1)) {
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
        goto fail;
    owner = ability->owner;
    stats = &owner->stats;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        goto fail;
    args.projectile = owner->projectile;
    args.y = 0;
    args.z = 0;
    height = stats->height - 0x12000;
    if (SceneProcSprites_CreateEntry(stats->position, height, owner->projectile, owner->direction,
                                     &ability->id) < 0) {
    fail:
        return 0;
    }
    CallbackList_SubtractStateTenValue(stats, ability, cost);
    ability->remaining = ability->duration;
    return 1;
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
        goto fail;
    owner = ability->owner;
    stats = &owner->stats;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        goto fail;
    args.projectile = owner->projectile;
    args.y = 0;
    args.z = 0;
    height = stats->height - 0x1a000;
    if (SpritePool_StartEntry(stats->position, height, 0, &ability->id, owner->direction) < 0) {
    fail:
        return 0;
    }
    CallbackList_SubtractStateTenValue(stats, ability, cost);
    ability->remaining = ability->duration;
    return 1;
}

s32 ActorAction_TrySpawn(struct ActorAbility *ability)
{
    struct ActorAbilityOwner *owner;
    struct ActorStats *stats;
    s32 cost;
    struct ActorAbilityProjectileParameters args;
    if (!Modifier_IsFourthSourceReady(ability))
        goto fail;
    owner = ability->owner;
    stats = &owner->stats;
    cost = CallbackList_RunMatchingState(stats, ability->cost, 10);
    if (stats->value < cost)
        goto fail;
    args.projectile = owner->projectile;
    args.y = 0;
    args.z = 0;
    ability->request = SpritePairPool_StartEntry(stats->position, &stats->height, 0, &ability->id,
                                                 owner->direction);
    if ((s8)ability->request < 0) {
    fail:
        return 0;
    }
    CallbackList_SubtractStateTenValue(stats, ability, cost);
    ability->remaining = ability->duration;
    return 1;
}

void Modifier_InitFourthSourceEffect(struct ActorStats *stats, u32 owner)
{
    u8 *selection = stats->state48;
    struct ActorAbility *ability = &stats->ability;
    u32 zero = 0;
    void (*callback)(void);
    void *config;
    ability->callback = (void (*)(void))zero;
    ability->enabled = zero;
    ability->owner = (void *)owner;
    config = &stats->ability.id;
    Memory_Clear(config, 0xc);
    if (ScriptRecord_FindAttributeModifier(config, (s8)selection[ACTOR_ABILITY_FOURTH_SOURCE]) <
        0) {
        ability->callback = (void (*)(void))CallbackStatus_ResetSelectorAndTest;
        ability->enabled = zero;
    } else {
        switch (ability->kind) {
            case 0:
                callback = (void (*)(void))ActorAbility_SpawnProjectile;
                goto set;
            case 1:
                callback = (void (*)(void))ActorAbility_ActivatePaletteEffect;
                goto set;
            case 2:
                callback = (void (*)(void))ActorAbility_SpawnAlternateProjectile;
                goto set;
            case 3:
                callback = (void (*)(void))ActorAction_TrySpawn;
                goto set;
            case 4:
                callback = (void (*)(void))ModifierAction_ActivateCost;
                goto set;
            case 5:
                callback = (void (*)(void))ActorAbility_UpdateRecovery;
            set:
                ability->callback = callback;
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
        u32 zero = ability->cooldown;
        if (!zero) {
            ability->cooldown = ability->interval;
            CallbackList_SubtractStateTenValue(stats, ability, 0);
            PalFade_Start(stats, 0x385, 0x385, (s32)target, stats->recoveryValue, 16, 4, zero);
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
