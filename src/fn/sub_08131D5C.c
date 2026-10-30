#include "global.h"
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
/* Tentative damage/HUD properties within the common actor record. */
struct SceneActorDamageProperties {
    u8 unknown0[0x9A];
    s16 hudParameter;
    u8 unknown9C[0x16];
    s16 hudKind;
    u8 unknownB4[0x12E];
    u16 damagePaletteBank;
    u8 unknown1E4[0x50];
    void *defeatControl;
};
struct SceneActorDamageDrawable {
    u8 unknown0[0x68];
    u8 *palette;
};
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag2D0(struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2D0 & mask;
}
static inline void Reaction_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                       void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void Hide(struct SceneActorRegistryResetView *a)
{
    if (!a->drawableKind) {
        u32 *flags = a->drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)a->drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}

void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
static inline u16 OwnershipFlagBits(u32 value, u32 mask)
{
    return value & mask;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 *owner = &actor->field2E0;
        u32 mask = *owner;
        if (!OwnershipFlagBits(mask, 0x20) && mask) {
            switch (LocalPlayer()) {
                case 0:
                    if (*owner != 1)
                        return 0;
                    break;
                case 1:
                    if (*owner != 2)
                        return 0;
                    break;
                case 2:
                    if (*owner != 4)
                        return 0;
                    break;
                case 3:
                    if (*owner != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

struct FlagHalfword {
    s16 bits : 16;
};
static inline void SetHitFlags(s16 bit, struct FlagHalfword *p)
{
    p->bits = bit | p->bits;
}
static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
s32 AreaEffect_ScaleGainAtPosition(void *, u32);
static inline u32 NextRandomSample(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((u32)table + index * 2);
}
static inline u32 ActorStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u32 GetReactionFlags(u32 **out, struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 *p = &actor->flags2CC;
    u32 result = *p & mask;
    *out = p;
    return result;
}
static inline struct ContactCollider *GetDamageCollider(struct SceneActorRegistryResetView *actor)
{
    return &actor->colliderA;
}
// Area-effect damage with a random knockback heading.
u8 SceneActorReaction_ApplyAreaDamage(struct SceneActorRegistryResetView *actor)
{
    s32 dx;
    struct ContactPosition *position;
    struct ContactCollider *displayCollider;
    s16 *health;
    u16 *timer;
    u16 *maxHealth;
    u32 *flags2CC;
    struct ContactCollider *collider;
    u32 *flags2D8;
    s32 gain;
    s32 dz;
    if ((ActorStatusFlags(actor, 8) || (actor->enabled != 1)) ||
        GetReactionFlags(&flags2CC, actor, 2))
        goto fail;
    collider = GetDamageCollider(actor);
    {
        u16 *p = &actor->colliderA.responseValue;
        u32 value = *p;
        displayCollider = collider;
        timer = p;
        if (value)
            goto fail;
    }
    {
        struct ContactPosition *pos = &actor->body.position;
        u16 *max = (u16 *)(&actor->defaultHealth);
        gain = AreaEffect_ScaleGainAtPosition(pos, *max);
        position = pos;
        maxHealth = max;
    }
    if (!gain)
        goto fail;
    dx = (NextRandomSample() & 0x3F) - 16;
    dz = (NextRandomSample() & 0x3F) - 16;
    collider->cost = gain;
    actor->reactionSoundVariant = gain;
    {
        u32 damage = collider->cost;
        s16 *h = &actor->health;
        u32 *f;
        u32 result;
        *h -= damage;
        f = &actor->field2D8;
        result = *f & 0x20000;
        health = h;
        flags2D8 = f;
        if (!result) {
            u16 bank = ((struct SceneActorDamageProperties *)actor)->damagePaletteBank;
            if (actor->drawableKind == 0)
                AnimData_SelectPaletteBank(((u8 *)actor->drawable) + 0x2C, bank);
            else
                ((struct SceneActorDamageDrawable *)actor->drawable)->palette =
                    gPaletteBankData + (bank * 32);
            {
                u32 eight = 8;
                actor->field1E8 = eight;
            }
            {
                u32 bit = 0x8000;
                *flags2CC |= bit;
            }
            if (ShouldShowDamage(actor)) {
                u32 zero;
                u32 buffer[2];
                s32 h = *health;
                u32 max = *maxHealth;
                s32 kind = ((struct SceneActorDamageProperties *)actor)->hudKind;
                s32 param = ((struct SceneActorDamageProperties *)actor)->hudParameter;
                SceneHud_SetTimedState(actor->body.id, h, max, kind, 0, param);
                Fill(&zero, buffer);
                if (actor->reactionSoundVariant || (!((*flags2D8) & 0x20000)))
                    DamageNumberDisplay_Show(displayCollider, actor->reactionSoundVariant, 0,
                                             position, buffer, 1);
            }
        }
    }
    SceneActorMovement_ProjectProbe(actor, dx, dz);
    SetHitFlags(8, (void *)&actor->flags2D4);
    actor->body.direction = Math_Atan2(dx, dz);
    *timer = 38;
    actor->field2DC = 0x40000;
    actor->field2D8 = 0;
    actor->field2B8 = 0;
    if ((*health) <= 0) {
        collider->responseValue = 0;
        Hide(actor);
        {
            void *control = ((struct SceneActorDamageProperties *)actor)->defeatControl;
            Reaction_SetControl(actor, 17, control);
        }
        {
            u32 state = 3;
            actor->state2A8 = state;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
        actor->field2E0 = 32;
        if ((!Test(gEngineFlags, 0x4800)) && (!Flag2D0(actor, 0x800)))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->body.id);
    } else {
        SetHitFlags(0x20, (void *)&actor->flags2D4);
        {
            void *control = actor->reactionTarget230;
            Reaction_SetControl(actor, 0x12, control);
        }
        {
            u32 state = 3;
            actor->state2A8 = state;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
    }
    {
        u32 mask = 0x2000;
        *flags2CC |= mask;
    }
    return 1;
fail:
    return 0;
}
