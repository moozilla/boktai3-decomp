#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Pointer spills, RNG load/store scheduling and branch layout still differ.
// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
extern u16 gUnk_030039F0;
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
void SceneActorReaction_PlayStaggerBurst(void);
extern u8 *gUnk_030042E4;
static inline s32 LocalPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gUnk_030053F4, 0x4800)) {
        register u32 *owner asm("r1") = &actor->field2E0;
        register u32 mask asm("r2") = *owner;
        if (!(mask & 0x20) && mask) {
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
static inline void ClearHit(struct FlagHalfword *flags, u32 mask)
{
    flags->bits = mask & flags->bits;
}
static inline void SetHit(u16 *p, u32 bit)
{
    *p = bit | *p;
}
static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
s32 AreaEffect_ScaleGainAtPosition(void *, u32);
s32 AreaEffect_ScaleLinkGainAtPosition(void *, u32);
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
static inline u32 RandomBits(u32 mask)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308] & mask;
}
static inline u32 ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u32 GetFlags(u32 **out, struct SceneActorRegistryResetView *actor, u32 mask)
{
    u32 *p = &actor->flags2CC;
    u32 result = *p & mask;
    *out = p;
    return result;
}
// Area-effect damage with a random knockback heading.
u8 SceneActorReaction_ApplyAreaDamage(struct SceneActorRegistryResetView *actor)
{
    u32 *flags2CC;
    struct ContactCollider *collider;
    u16 *timer;
    struct ContactPosition *position;
    u16 *maxHealth;
    s16 *health;
    u32 *flags2D8;
    s32 gain;
    s32 dx;
    register s32 dz asm("sl");
    typeof(collider) c1;
    if ((ActorFlags(actor, 8) || (actor->enabled != 1)) || GetFlags(&flags2CC, actor, 2))
        goto fail;
    collider = &actor->colliderA;
    c1 = collider;
    timer = (u16 *)(((u8 *)actor) + 0x178);
    if (*timer)
        goto fail;
    {
        register struct ContactPosition *pos asm("r4") = &actor->position;
        register u16 *max asm("r5") = (u16 *)(&actor->defaultHealth);
        gain = AreaEffect_ScaleGainAtPosition(pos, *max);
        asm("" : "+r"(pos), "+r"(max));
        position = pos;
        maxHealth = max;
    }
    if (!gain)
        goto fail;
    dx = RandomBits(0x3F) - 16;
    dz = RandomBits(0x3F) - 16;
    c1->cost = gain;
    actor->reactionSoundVariant = gain;
    {
        u32 damage = c1->cost;
        health = &actor->health;
        *health -= damage;
    }
    flags2D8 = &actor->field2D8;
    if (!((*flags2D8) & 0x20000)) {
        u16 bank = *((u16 *)(((u8 *)actor) + 0x1E2));
        if (actor->drawableKind == 0)
            AnimData_SelectPaletteBank(((u8 *)actor->drawable) + 0x2C, bank);
        else
            *((u8 **)(((u8 *)actor->drawable) + 0x68)) = gUnk_030042E4 + (bank * 32);
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
            s32 kind = *((s16 *)(((u8 *)actor) + 0xB2));
            s32 param = *((s16 *)(((u8 *)actor) + 0x9A));
            SceneHud_SetTimedState(actor->id, h, max, kind, 0, param);
            Fill(&zero, buffer);
            if (actor->reactionSoundVariant || (!((*flags2D8) & 0x20000)))
                DamageNumberDisplay_Show(c1, actor->reactionSoundVariant, 0, position, buffer, 1);
        }
    }
    SceneActorMovement_ProjectProbe(actor, dx, dz);
    {
        struct FlagHalfword *flags = (void *)(&actor->flags2D4);
        flags->bits |= 8;
    }
    actor->direction = Math_Atan2(dx, dz);
    *timer = 38;
    actor->field2DC = 0x40000;
    actor->field2D8 = 0;
    actor->field2B8 = 0;
    if ((*health) <= 0) {
        collider->responseValue = 0;
        Hide(actor);
        {
            void *control = *((void **)(((u8 *)actor) + 0x234));
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
        if ((!Test(gUnk_030053F4, 0x4800)) && (!Flag2D0(actor, 0x800)))
            SceneActorRecord_RecordPair(gUnk_030039F0, actor->id);
    } else {
        {
            register u32 bit asm("r0") = 0x20;
            asm("" : "+r"(bit));
            actor->flags2D4 = bit | actor->flags2D4;
        }
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

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_ApplyAreaDamage);
#endif
