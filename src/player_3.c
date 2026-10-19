#include "player_3.h"

/* Tentative role: select an effect flag from actor and global state. */

static inline u8 global_mode(void)
{
    u32 mask = 2;
    if (gUnk_02000710->flags868 & mask)
        return 1;
    return 0;
}

s32 Player_GetVisualEffectFlags(struct Player *s)
{
    s32 result = 0;
    if (s->actionMode == 2)
        return 0;
    if (!global_mode()) {
        if (s->statusFlags & 16)
            result = 8;
    } else {
        if (s->statusFlags & 16)
            result = 4;
    }
    return result;
}

void Player_LoadVisualParameters(struct Player *p)
{
    u16 *t;
    s32 i;
    if (p->baseActionVariant <= 9) {
        t = (u16 *)(gUnk_030042E4 + 0x6A0);
        i = p->baseActionVariant;
    } else {
        t = (u16 *)(gUnk_030042E4 + 0x2F60);
        i = p->baseActionVariant;
        i -= 10;
    }
    t += i * 3;
    p->visualParameterA = *t++;
    p->visualParameterB = *t;
    p->visualParameterC = t[1];
}

INCLUDE_ASM("asm/nonmatching", sub_0813C9B4);

INCLUDE_ASM("asm/nonmatching", sub_0813CB8C);

INCLUDE_ASM("asm/nonmatching", sub_0813CE20);

void Player_SelectSpriteResource(struct Player *p)
{
    if (p->slot == 0) {
        p->baseSpriteResource = 0x2A;
    } else {
        p->baseSpriteResource = 0x35;
    }
}

void sub_0813D178(void *);

void sub_0813CB8C(void *, u32);

INCLUDE_ASM("asm/nonmatching", sub_0813D178);

void Player_InitVisualState(struct Player *p)
{
    Player_SelectSpriteResource(p);
    sub_0813D178(p);
    p->savedSpriteResource = 0xFFFF;
    p->baseActionVariant = 0xFF;
    p->spriteResource = p->baseSpriteResource;
    p->actionVariantResource = (u32)p->visualParameters;
    p->visualObject->resource = p->baseSpriteResource;
    p->visualObject->parameters = (u32)p->visualParameters;
    sub_0813CB8C(p, 0);
}

void SceneActor_StartRandomizedEffect(struct Player *);

INCLUDE_ASM("asm/nonmatching", SceneActor_StartRandomizedEffect);

void Player_EmitQueuedEffects(struct Player *p)
{
    u8 t = p->movementModifierActive;
    s32 n;
    if (t != 0) {
        n = t;
        do {
            SceneActor_StartRandomizedEffect(p);
            n--;
        } while (n != 0);
    }
}

void EffectGroup_Reset(u8 *);

void Player_FlushQueuedEffects(struct Player *p)
{
    if (p->movementModifierKind == 1) {
        EffectGroup_Reset(&p->effectGroup);
    } else {
        u8 t = p->movementModifierActive;
        s32 n;

        if (t != 0) {
            n = t;
            do {
                SceneActor_StartRandomizedEffect(p);
                n--;
            } while (n != 0);
        }
    }
}

INCLUDE_ASM("asm/nonmatching", sub_0813D548);

s32 Player_GetEffectTickDelta(struct Player *p)
{
    s32 r = p->facingChangeEffects;
    r -= p->savedEffectTick;
    r += 0x100;
    return r & 0xFF;
}

#define SceneActor_GetEffectElapsedTicks Player_GetEffectTickDelta

#define gSceneActorEffectThresholds gUnk_08610918

extern const u8 gUnk_08610918[];

void SceneActor_StartRandomizedEffect(struct Player *);

s32 Player_GetEffectTickDelta(struct Player *);

s32 SceneActor_SelectEffectDisplayMode(struct Player *p, s32 mode)
{
    s32 original = mode;
    if (p->movementModifierActive) {
        if (p->kind & 1) {
            if (Player_GetEffectTickDelta(p) >= gUnk_08610918[p->movementModifierKind - 1]) {
                SceneActor_StartRandomizedEffect(p);
                p->savedEffectTick = p->facingChangeEffects;
            }
        }
        if (p->movementModifierActive) {
            if (p->movementModifierKind == 1)
                mode = 3;
            else if (p->movementModifierKind == 2)
                mode = 4;
            else if (p->movementModifierKind == 3)
                mode = 5;
        }
    }
    if (p->movementSpeedReduction)
        mode = 7;
    if (mode == original) {
        if (p->visualFlashTimer) {
            if (((u16)p->visualFlashTimer >> 2) & 1)
                mode = p->savedVisualMode;
            p->visualFlashTimer--;
        }
    } else {
        p->savedVisualMode = mode;
        p->visualFlashTimer = 64;
    }
    return mode;
}

INCLUDE_ASM("asm/nonmatching", sub_0813D834);

void Player_ClearStateTimer(struct Player *p, s32 idx)
{
    p->scriptStatus[idx] = 0;
    if (idx == 1) {
        u16 v = gUnk_02000710->values08[4];
        p->catalogSavedOption = v;
    }
}

INCLUDE_ASM("asm/nonmatching", sub_0813D924);

#ifdef NONMATCHING
// fuzzy: 82.1%
// Register allocation swaps the preserved actor and input mode.
s32 Player_SelectFlashingVisualMode(struct Player *p, s32 mode)
{
    s32 result = mode;
    if (p->movementRequestScale) {
        if ((--p->movementRequestScale >> 2) & 1)
            goto flash;
    }
    return result;
flash:
    return p->movementRequestParameter;
}
#else
INCLUDE_ASM("asm/nonmatching", Player_SelectFlashingVisualMode);
#endif

INCLUDE_ASM("asm/nonmatching", sub_0813DB98);

void Player_AdvanceEffectTick(struct Player *p)
{
    const u16 *q = p->inputKeys;

    if (q[1] & 0xF3) {
        p->facingChangeEffects++;
    }
}

#define SceneActor_AdvanceEffectTick Player_AdvanceEffectTick

void sub_0813DB98(void *);

void Player_AdvanceEffectTick(struct Player *);

s32 sub_0813D924(void *, s32);

s32 SceneActor_SelectEffectDisplayMode(struct Player *, s32);

s32 Player_SelectFlashingVisualMode(struct Player *, s32);

void sub_0813CE20(void *, s32);

void sub_0813B950(void *);

void Player_UpdateOrbitingEffect(void *);

void sub_0813BAD8(void *);

void ConvergingEffects_UpdateParticles(void *);

void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);

void SceneActor_UpdateVisualEffects(struct Player *p)
{
    s32 mode;
    sub_0813DB98(p);
    Player_AdvanceEffectTick(p);
    mode = sub_0813D924(p, 0);
    mode = SceneActor_SelectEffectDisplayMode(p, mode);
    mode = Player_SelectFlashingVisualMode(p, mode);
    if (p->kind == 2 && p->tetherMode != 10 && p->tetherMode != 11)
        sub_0813CE20(p, mode);
    else
        sub_0813CB8C(p, mode);
    sub_0813B950(p);
    Player_UpdateOrbitingEffect(p);
    sub_0813BAD8(p);
    ConvergingEffects_UpdateParticles(p);
    {
        void **callback = &p->directionalEffectCallback;
        void *context = p->directionalEffect;
        Callback_InvokeActor(context, *callback);
    }
    if (p->kindTwoAttachmentCounter) {
        if (--p->kindTwoAttachmentCounter == 0)
            p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor,
                127, 0, 0, 0, 80);
    }
    if (p->tetherMode == 38 || p->tetherMode == 12) {
        p->effectPosition[0] = p->alternatePosition[0] + p->resetValue6F8;
        p->effectPosition[1] = p->alternatePosition[1] + p->resetValue6FA;
        p->effectPosition[2] = p->alternatePosition[2] + p->resetValue6FC;
    } else {
        p->effectPosition[0] = p->position.x + p->resetValue6F8;
        p->effectPosition[1] = p->position.y + p->resetValue6FA;
        p->effectPosition[2] = p->position.z + p->resetValue6FC;
    }
}

void SceneActorEffects_DestroyGroup(void *);

void Shadow_Destroy(void *);

void ConvergingEffects_RemoveSprites(void *);

void DirectionalSpriteEffect_RemoveSprite(void *);

void Player_DestroyEffects(struct Player *p)
{
    SceneActorEffects_DestroyGroup(&p->effectGroup);
    Shadow_Destroy(p->shadow);
    Effect_Remove(p->visualEffects748);
    Effect_Remove(p->visualEffects778);
    Effect_Remove(p->visualEffects7A0);
    Effect_Remove(p->visualEffects7D0);
    ConvergingEffects_RemoveSprites(p);
    DirectionalSpriteEffect_RemoveSprite(p);
}

/* Player_GetEffectIntensity: Returns the global signed value also used to pace inward particle emission; intensity interpretation tentative */

#define Player_GetEffectIntensity sub_0813E0F0

INCLUDE_ASM("asm/nonmatching", sub_0813DF5C);

s16 sub_0813E0F0(void)
{
    return gUnk_02000710->unk876;
}

s32 Div(s32, s32);

#define Player_GetStateModifierBonus sub_0813E108
s32 Player_GetStateModifierBonus(struct Player *p)
{
    s32 i;
    for (i = 0; i <= 1; i++) {
        if (p->scriptStatus[i])
            return 10;
    }
    if (*(s32 *)((u8 *)gUnk_02000710 + p->timedEventSlot * 4 + 0x628))
        return 10;
    if (p->movementModifierActive)
        return 10;
    return 0;
}
#define Player_GetBlockedActionBonus sub_0813E160
s32 Player_IsActionBlocked(struct Player *);
s32 Player_GetBlockedActionBonus(struct Player *p)
{
    if (Player_IsActionBlocked(p) == 0)
        goto zero;
    return 10;
zero:
    return 0;
}
#define Player_GetStatAModifierBonus sub_0813E174
s32 Player_GetStatAModifierBonus(struct Player *p)
{
    return p->derivedStats[0] >> 3;
}
#define Player_GetStatBModifierBonus sub_0813E184
s32 Player_GetStatBModifierBonus(struct Player *p)
{
    return p->derivedStats[1] >> 3;
}
#define Player_GetEnergyRatioModifier sub_0813E194
s32 Player_GetEnergyRatioModifier(struct Player *p)
{
    return Div(p->currentStatB * 10, p->maximumStatB);
}
#define Player_GetLifeRatioModifier sub_0813E1B8
s32 Player_GetLifeRatioModifier(void)
{
    return Div(gUnk_02000710->life * 10, gUnk_02000710->lifeMax);
}

s32 Player_ModifyValueByMissingLife(void *unused, s32 n)
{
    s32 a = Div(n, 4);
    s16 v1 = gUnk_02000710->lifeMax;
    s32 b = Div(v1 - gUnk_02000710->life, v1);
    return a + n * b;
}

s32 Player_ModifyValueByRemainingLife(void *unused, s32 n)
{
    s32 a = Div(n, 10);
    s32 b = Div(n, 2);
    s16 v1 = gUnk_02000710->lifeMax;
    s32 c = Div(v1 - gUnk_02000710->life, v1);
    return n + a - b * c;
}

u8 *Equipment_GetDefinition(u8);

s32 Player_ModifyValueByEquipmentLevel(struct PlayerModifierInput *p, s32 n)
{
    u8 *t = Equipment_GetDefinition(p->equipment);
    s32 a = Div(Div(n, 2), 100 - t[2]);
    s32 b = gUnk_02000710->level - t[2];

    return n - b * a;
}

#define Player_GetScaledTargetFlagBonus sub_0813E284
static inline s32 LimitTargetBonus(s32 value)
{
    if (value > 10)
        value = 10;
    return value;
}
s32 Player_GetScaledTargetFlagBonus(s32 unused0, s32 unused1, struct PlayerModifierTarget *target)
{
    u32 mask = 0x80;
    if (target->flags & mask)
        return LimitTargetBonus(gUnk_02000710->targetFlagBonuses[0] >> 6);
    else if (target->flags & 0x100)
        return LimitTargetBonus(gUnk_02000710->targetFlagBonuses[1] >> 6);
    else if (target->flags & 0x200)
        return LimitTargetBonus(gUnk_02000710->targetFlagBonuses[2] >> 6);
    else if (target->flags & 0x400)
        return LimitTargetBonus(gUnk_02000710->targetFlagBonuses[3] >> 6);
    else if (target->flags & 0x800)
        return LimitTargetBonus(gUnk_02000710->targetFlagBonuses[4] >> 6);
    return 0;
}

s32 Mod(s32, s32);

s32 Player_RollModifierBonus(void)
{
    s32 r;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    r = Mod(*(const u16 *)((const u8 *)gUnk_0203B400 + (gUnk_03005308 << 1)), 100);
    if (r > 10) {
        r = 0;
    } else {
        r = 10;
    }
    return r;
}

s32 Player_GetTargetFlag7Bonus(s32 unused0, s32 unused1, struct PlayerModifierTarget *p)
{
    u32 m = 0x80;
    if (p->flags & m) {
        return 10;
    }
    return 0;
}

s32 Player_GetTargetFlag8Bonus(s32 unused0, s32 unused1, struct PlayerModifierTarget *p)
{
    u32 m = 0x80;
    m <<= 1;
    if (p->flags & m) {
        return 10;
    }
    return 0;
}

s32 Player_GetTargetFlag9Bonus(s32 unused0, s32 unused1, struct PlayerModifierTarget *p)
{
    u32 m = 0x80;
    m <<= 2;
    if (p->flags & m) {
        return 10;
    }
    return 0;
}

s32 Player_GetTargetFlag10Bonus(s32 unused0, s32 unused1, struct PlayerModifierTarget *p)
{
    u32 m = 0x80;
    m <<= 3;
    if (p->flags & m) {
        return 10;
    }
    return 0;
}

s32 Player_GetTargetFlag11Bonus(s32 unused0, s32 unused1, struct PlayerModifierTarget *p)
{
    u32 m = 0x80;
    m <<= 4;
    if (p->flags & m) {
        return 10;
    }
    return 0;
}

s32 Player_OnWeaponHitApplySol(struct Player *p)
{
    p->actionVariant = 1;
    return 1;
}

s32 Player_OnWeaponHitApplyFlame(struct Player *p)
{
    p->actionVariant = 3;
    return 4;
}

s32 Player_OnWeaponHitApplyFrost(struct Player *p)
{
    p->actionVariant = 4;
    return 8;
}

s32 Player_OnWeaponHitApplyCloud(struct Player *p)
{
    p->actionVariant = 5;
    return 16;
}

s32 Player_OnWeaponHitApplyEarth(struct Player *p)
{
    p->actionVariant = 6;
    return 32;
}

s32 Player_OnWeaponHitRollEffect1000(void)
{
    s32 r;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    r = Mod(*(const u16 *)((const u8 *)gUnk_0203B400 + (gUnk_03005308 << 1)), 100);
    if (r > 10) {
        r = 0;
    } else {
        r = 0x1000;
    }
    return r;
}

s32 Player_OnWeaponHitRollEffect40000Low(void)
{
    s32 r;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    r = Mod(*(const u16 *)((const u8 *)gUnk_0203B400 + (gUnk_03005308 << 1)), 100);
    if (r > 10) {
        r = 0;
    } else {
        r = 0x40000;
    }
    return r;
}

s32 Player_OnWeaponHitRollEffect40000High(void)
{
    s32 r;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    r = Mod(*(const u16 *)((const u8 *)gUnk_0203B400 + (gUnk_03005308 << 1)), 100);
    if (r > 20) {
        r = 0;
    } else {
        r = 0x40000;
    }
    return r;
}

s32 Player_OnWeaponHitRollEffect80000Low(void)
{
    s32 r;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    r = Mod(*(const u16 *)((const u8 *)gUnk_0203B400 + (gUnk_03005308 << 1)), 100);
    if (r > 10) {
        r = 0;
    } else {
        r = 0x80000;
    }
    return r;
}

s32 Player_OnWeaponHitRollEffect80000High(void)
{
    s32 r;

    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    r = Mod(*(const u16 *)((const u8 *)gUnk_0203B400 + (gUnk_03005308 << 1)), 100);
    if (r > 20) {
        r = 0;
    } else {
        r = 0x80000;
    }
    return r;
}

#define Player_AddStatA Player_IncreaseLifeTowardLimit

#define Player_SubtractStatA Player_SubtractLifeNonlethal

#define Player_AddStatB Player_AddEnergy

#define Player_SubtractStatB Player_SubtractEnergy

void Player_IncreaseLifeTowardLimit(struct Player *, s32);

void Player_SubtractLifeNonlethal(struct Player *, s32);

void Player_AddEnergy(struct Player *, s32);

void Player_SubtractEnergy(struct Player *, s32);

static inline u32 TestFlags(u32 *flags, u32 mask)
{
    u32 m = mask;
    return *flags & m;
}

static inline s32 HasFlag(struct Player *p, u32 mask)
{
    if (!p)
        return FALSE;
    if (TestFlags(&p->statBonusFlags, mask))
        return TRUE;
    return FALSE;
}

void Player_ApplyPeriodicStatChanges(struct Player *p)
{
    s32 amount;
    if (p->kind == 1) {
        if (HasFlag(p, 4)) {
            amount = Div(p->maximumStatA, 100);
            if (amount <= 0)
                amount = 1;
            Player_IncreaseLifeTowardLimit(p, amount);
        }
        if (HasFlag(p, 8)) {
            amount = Div(p->maximumStatA, 100);
            if (amount <= 0)
                amount = 1;
            Player_SubtractLifeNonlethal(p, amount);
        }
        if (HasFlag(p, 16)) {
            amount = Div(p->maximumStatB, 100);
            if (amount <= 0)
                amount = 1;
            Player_AddEnergy(p, amount);
        }
        if (HasFlag(p, 32)) {
            amount = Div(p->maximumStatB, 100);
            if (amount <= 0)
                amount = 1;
            Player_SubtractEnergy(p, amount);
        }
    }
}

INCLUDE_ASM("asm/nonmatching", Player_BuildActionModifiers);

INCLUDE_ASM("asm/nonmatching", sub_0813E90C);

void Player_SetWeaponInstance(struct Player *p, u32 v)
{
    p->statModifierContext.actionEntry = (struct PlayerActionEntry *)v;
}

#define Player_SetActionEntry Player_SetWeaponInstance

#define gPlayerActionDescriptors gUnk_08E6092C

extern const struct PlayerActionDescriptor gUnk_08E6092C[];

void Player_SetWeaponInstance(struct Player *, u32);

void ItemDescriptor_Reset(void *);

void ActionRecord_CopyFromEntry(void *, void *);

void Player_BuildActionModifiers(struct Player *, struct PlayerStatModifierContext *, const void *);

void sub_0813E90C(struct Player *);

void sub_0814F328(void);

void sub_0814D9C4(void);

void ActorSequence_Begin(struct Player *, s32, s32);

void VariantSprite_SelectAnimation(void *, u32);

void Player_BeginDescriptorAction(struct Player *p, u32 entry)
{
    struct PlayerActionDescriptor descriptor;
    if (p->actionMode != 6) {
        Player_SetWeaponInstance(p, entry);
        if (p->statModifierContext.actionEntry == NULL)
            ItemDescriptor_Reset(&descriptor);
        else
            ActionRecord_CopyFromEntry(p->statModifierContext.actionEntry, &descriptor);
    } else {
        p->statModifierContext.actionEntry = NULL;
        descriptor = gUnk_08E6092C[47];
    }
    p->statModifierContext.actionKind = descriptor.kind;
    p->statModifierContext.statBOverrideEnabled = 0;
    p->statModifierContext.actionDuration = descriptor.duration;
    p->statModifierContext.actionPhase =
        Player_GetEquipmentValue((const struct SceneInputItemDescriptor *)&descriptor);
    Player_BuildActionModifiers(p, &p->statModifierContext, &descriptor);
    if (p->statModifierContext.actionKind == 0x30)
        p->actionCallback = sub_0814F328;
    else
        p->actionCallback = sub_0814D9C4;
    p->actionResetCounter = 0;
    sub_0813E90C(p);
    if (p->tetherMode == 3)
        ActorSequence_Begin(p, 0, 0);
    if (p->statModifierContext.actionKind != 0x30)
        VariantSprite_SelectAnimation(
            p->actionDisplay, gUnk_08E6092C[p->statModifierContext.actionKind].displaySelection);
}

#define gPlayerIndexedActions gUnk_08E60F4C

extern const struct PlayerIndexedAction gUnk_08E60F4C[];

void sub_0814A9D4(void);

void sub_0814EEE0(void);

void sub_0814E990(void);

void sub_0814E3D8(void);

static inline void ClearFlag(struct Player *p, u32 index)
{
    if (!p)
        return;
    if (index > 63)
        p->actionStateFlags &= ~(1 << (index - 64));
    else if (index > 31)
        p->actionFlags &= ~(1 << (index - 32));
    else
        p->statBonusFlags &= ~(1 << index);
}

void Player_BeginIndexedAction(struct Player *p, s32 action)
{
    s32 index = action - 1;
    const struct PlayerIndexedAction *table = gUnk_08E60F4C;
    const struct PlayerIndexedAction *descriptor;
    s32 i;
    if (p) {
        for (i = 0; i <= 6; i++)
            ClearFlag(p, i);
    }
    if (p->actionMode != 6)
        descriptor = &table[index];
    else
        descriptor = &table[index];
    p->statModifierContext.actionKind = action;
    p->statModifierContext.actionDuration = descriptor->duration;
    p->statModifierContext.statBOverrideEnabled = 4;
    if (p->actionMode == 6)
        p->actionCallback = sub_0814A9D4;
    else if (p->statModifierContext.actionKind == 11)
        p->actionCallback = sub_0814EEE0;
    else if (p->statModifierContext.actionKind == 8)
        p->actionCallback = sub_0814E990;
    else
        p->actionCallback = sub_0814E3D8;
    sub_0813E90C(p);
    if (p->tetherMode == 3)
        ActorSequence_Begin(p, 0, 0);
}

void Player_SetWeaponMode(struct Player *p, u8 v)
{
    p->statModifierContext.statBOverrideMode = v;
}

void Hud_BeginModeSixCue(void);

void Player_BeginDescriptorAction(struct Player *, u32);

void Player_ConsumeActionCharge(struct Player *p)
{
    s32 cost;
    struct PlayerActionEntry *entry;

    if (p->actionMode != 6) {
        {
            struct PlayerActionDrainContext *drain = &p->statModifierContext.drain;
            cost = drain->pendingCost;
        }
        if (cost > 0) {
            if (!p->statModifierContext.statBOverrideEnabled) {
                s32 kind = p->statModifierContext.actionEntry->kind;
                if (kind == 0x30)
                    goto clear;
                if (kind == 0x2F)
                    goto clear;
                {
                    if (HasFlag(p, 0x400))
                        cost <<= 1;
                    if (HasFlag(p, 1) || HasFlag(p, 0x200))
                        cost >>= 1;
                    if (cost <= 0)
                        cost = 1;
                    entry = p->statModifierContext.actionEntry;
                    if (entry->remainingCharge != 0) {
                        if (entry->remainingCharge <= cost) {
                            entry->remainingCharge = 0;
                            Hud_BeginModeSixCue();
                            Player_BeginDescriptorAction(p,
                                                         (u32)p->statModifierContext.actionEntry);
                        } else
                            entry->remainingCharge -= cost;
                    }
                }
            }
        clear: {
            struct PlayerActionDrainContext *drain = &p->statModifierContext.drain;
            drain->pendingCost = 0;
        }
        }
    }
}

void Player_RefreshEquipmentWithOptions(struct Player *p, s32 unused, s32 flag);

INCLUDE_ASM("asm/nonmatching", PlayerAction_RefreshFlagState);

INCLUDE_ASM("asm/nonmatching", sub_0813F702);

void Player_RefreshEquipment(struct Player *p, s32 q)
{
    Player_RefreshEquipmentWithOptions(p, q, -1);
}

void PlayerAction_RefreshFlagState(struct Player *, u8 *);

void Player_RefreshStatsAndVitals(struct Player *);

void Player_RefreshEquipmentWithOptions(struct Player *p, s32 unused, s32 flag)
{
    PlayerAction_RefreshFlagState(p, p->actionFlagState);
    Player_RefreshStatsAndVitals(p);
    p->movementStatTimer = 0;
}

void Player_InitEquipmentHook(void)
{
}

extern const u16 gUnk_0861091C[], gUnk_08610940[];

u32 ActorParameter_GetModeValue(s32 index)
{
    if (gUnk_030053F4 & 0x1000)
        return gUnk_08610940[index];
    else
        return gUnk_0861091C[index];
}

static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}

u32 ActorParameter_GetModeValue(s32);

s32 Player_GetEnchantmentEnergyCost(struct Player *p)
{
    s32 v;
    s32 pct;
    if ((s8)p->thresholdMode < 0)
        return 0;
    v = ActorParameter_GetModeValue((s8)p->thresholdMode);
    pct = 100;
    if (p != 0 && tst(&p->actionFlags, 0x1000))
        pct = 50;
    if (pct <= 99)
        v = Div(v * pct, 100);
    return v;
}

s32 Player_GetEnchantmentEnergyCost(struct Player *p);

s32 Player_CanAffordEnchantment(struct Player *p)
{
    const u16 *q;
    s32 r = Player_GetEnchantmentEnergyCost(p);

    q = &p->currentStatB;
    if (*(u16 *)q >= r) {
        return 1;
    }
    return 0;
}

#define Player_SpendEnchantmentEnergy sub_0813F828
void Player_SpendEnchantmentEnergy(struct Player *p)
{
    s32 cost = Player_GetEnchantmentEnergyCost(p);
    u16 *energy = &p->currentStatB;
    if (*energy < cost)
        *energy = 0;
    else
        *energy -= cost;
}

s32 Player_IsEnchantmentAvailable(struct Player *unused, s32 kind)
{
    return 1;
}

s32 Player_CanAffordEnchantment(struct Player *);

s32 Player_GetUsableEnchantment(struct Player *p)
{
    if (p->actionSelectionStamp != 0 && Player_CanAffordEnchantment(p) != 0 &&
        (u8)(p->statModifierContext.statBOverrideEnabled - 3) > 1)
        return (s8)p->thresholdMode;
    return -1;
}

s32 Player_HasRequiredEnergy(struct Player *p, s32 v)
{
    if ((s32)p->currentStatB < v) {
        return 0;
    }
    return 1;
}

s32 Player_CanSpendSharedResource(struct Player *p, s32 v)
{
    if (*(s32 *)((u8 *)gUnk_02000710 + p->timedEventSlot * 4 + 0x628) <= 0 &&
        gUnk_02000710->unk876 >= v)
        return 1;
    return 0;
}

#define Player_IsActionRequestEnabled Player_IsEnchantmentAvailable

void PlayerBarrier_Hide(void *);

s32 PlayerBarrier_IsActive(void *);

static inline void ClearModeFlag(struct Player *p)
{
    if (p) {
        u32 *flags = &p->actionStateFlags;
        u32 mask = ~16;
        *flags &= mask;
    }
}

static inline void SetModeFlag(struct Player *p)
{
    if (p) {
        u32 *flags = &p->actionStateFlags;
        u32 mask = 16;
        *flags |= mask;
    }
}

#ifdef NONMATCHING
// fuzzy: 85.0%
// Preserved owner/request registers differ; one exit reloads the mode.
s32 Player_SelectRequestedActionMode(struct Player *p)
{
    struct PlayerActionRequest *request = (struct PlayerActionRequest *)&p->thresholdMode;
    if (request->kind >= 0 && request->enabled && !request->blocked) {
        if (p->tetherMode > 1)
            return p->tetherMode;
        {
            switch (request->kind) {
                case 3:
                    if (gUnk_030053F4 & 0x1000)
                        return p->tetherMode;
                    return 19;
                case 2:
                    if (p->actionRequestRestricted == 1) {
                        Sound_PlaySong(0x192);
                        return p->tetherMode;
                    }
                    if (gUnk_030053F4 & 0x1000)
                        return p->tetherMode;
                    return 10;
                case 1:
                    if (p->actionRequestRestricted == 1) {
                        Sound_PlaySong(0x192);
                        return p->tetherMode;
                    }
                    if (gUnk_030053F4 & 0x1000)
                        return p->tetherMode;
                    return 11;
                case 0:
                    if (p->actionMode == 0 || p->actionMode == 6)
                        return 2;
                    return p->tetherMode;
            }
        }
    }
    return p->tetherMode;
}

#else
INCLUDE_ASM("asm/nonmatching", Player_SelectRequestedActionMode);
#endif

void Player_SetActionRequest(struct Player *p, s32 kind)
{
    struct PlayerActionRequest *request = (struct PlayerActionRequest *)&p->thresholdMode;
    if (request->kind != kind) {
        if (request->latched) {
            request->latched = 0;
            p->actionVariant = 0;
        }
        request->kind = kind;
        ClearModeFlag(p);
        if (kind == 3)
            SetModeFlag(p);
        else if (PlayerBarrier_IsActive(p->actionRequestEffect))
            PlayerBarrier_Hide(p->actionRequestEffect);
        if ((u8)request->kind > 5) {
            request->latched = 0;
            request->enabled = 0;
            request->duration = 0;
            p->actionRequestState = 0;
            if (p->actionRequestHold == 0)
                p->actionRequestTimer = 0;
        } else {
            request->enabled = Player_IsEnchantmentAvailable(p, request->kind);
            request->duration = ActorParameter_GetModeValue(request->kind);
            if (!request->enabled)
                return;
            request->latched = 0;
            p->actionRequestState = 0;
            if (p->actionRequestHold == 0)
                p->actionRequestTimer = 0;
        }
        p->actionVariant = p->baseActionVariant;
    }
}

extern s16 gUnk_02000554;

s32 Player_GetSelectedAbility(struct Player *p)
{
    if (p->actionMode != 6)
        /* Add the slot index before the field offset to retain the original address lifetime. */
        return *(s16 *)((u8 *)gUnk_02000710 + gUnk_02000710->selected * 2 + 0x88);
    gUnk_02000554 = 0;
    if (gUnk_02000710->abilityVariant == 0)
        return 4;
    return 5;
}

void Player_InitActionRequest(struct Player *p)
{
    struct PlayerActionRequest *request = (struct PlayerActionRequest *)&p->thresholdMode;
    if (gUnk_030053F4 & 0x1000)
        request->kind = 0xFF;
    else
        request->kind = Player_GetSelectedAbility(p);
    ClearModeFlag(p);
    if (request->kind == 3)
        SetModeFlag(p);
    if (request->kind < 0) {
        request->duration = 0;
        request->enabled = 0;
        request->latched = 0;
    } else {
        request->duration = ActorParameter_GetModeValue(request->kind);
        request->enabled = Player_IsEnchantmentAvailable(p, request->kind);
        request->latched = gUnk_02000554;
        if (!request->enabled)
            request->latched = 0;
    }
}

void Player_InitSlotFromScript(struct Player *p)
{
    s32 v;

    v = Script_SeekToKeyword(0x69);
    if (v != 0) {
        v = Script_GetValue();
    }
    p->slot = v;
}

void Player_ClearDirectionHistory(struct Player *p)
{
    s32 i;
    for (i = 0; i < 10; i++)
        p->directionHistory[i] |= 0xFFFF;
}

void Player_InitEventProcedureFromScript(struct Player *p)
{
    s32 v;

    v = Script_SeekToKeyword(0x52);
    if (v != 0) {
        p->eventProcedure = Script_GetValue();
    } else {
        p->eventProcedure = v;
    }
}
