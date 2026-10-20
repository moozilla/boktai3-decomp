#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
#ifdef NONMATCHING
// fuzzy: 83.5%
// Remaining differences: pointer spills, RNG temporaries and control-reset allocation.
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
s32 Math_Atan2(s32, s32);
// fuzzy: 0%
void SceneActorRecord_RecordPair(u16, u16);
extern u16 gUnk_030039F0;
extern u8 *gUnk_030042E4;
void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 HasPendingHit(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2E6 == 0)
        return 0;
    return 1;
}
static inline s32 LocalPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gUnk_030053F4, 0x4800)) {
        u32 *owner = &actor->field2E0;
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

static inline u32 Flag2D0(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline void Fill(u32 *zero, void *display)
{
    *zero = 0;
    CpuSet(zero, display, 0x05000002);
}
#define SceneActorReaction_ApplyAreaDamage sub_08132904
// Pending damage selects defeat control or the stagger reaction.
void SceneActorReaction_PlayStaggerBurst(void *);
u8 SceneActorReaction_ApplyAreaDamage(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    s32 dx, dz;
    u32 *flags2CC;
    u32 *flags2D8;
    s16 *healthp;
    u16 *timer;
    u16 *maxp;
    void *position;
    s32 damage;
    u32 zero;
    u32 display[2];
    register u8 *collider asm("r8");
    if (Flag2D0(actor, 8))
        goto fail;
    if (*((u8 *)actor + 0x2B4) != 1)
        goto fail;
    {
        u32 mask = 2;
        u32 *p = &actor->flags2CC;
        u32 v = *p & mask;
        flags2CC = p;
        if (v)
            goto fail;
    }
    collider = (u8 *)&actor->colliderA;
    timer = (u16 *)((u8 *)actor + 0x178);
    if (*timer)
        goto fail;
    if (Test(gUnk_030053F4, 0x4800)) {
        void *pos = &actor->position;
        u16 *maximum = (u16 *)((u8 *)actor + 0xB0);
        damage = AreaEffect_ScaleLinkGainAtPosition(pos, *maximum);
        position = pos;
        maxp = maximum;
    } else {
        void *pos = &actor->position;
        u16 *maximum = (u16 *)((u8 *)actor + 0xB0);
        damage = AreaEffect_ScaleGainAtPosition(pos, *maximum);
        position = pos;
        maxp = maximum;
    }
    if (!damage)
        goto fail;
    {
        const u16 *table = gUnk_0203B400;
        u32 *seed = &gUnk_03005308;
        u32 index = *seed + 1;
        u32 mask = 1023;
        u32 bits = 63;
        index &= mask;
        dx = (table[index] & bits) - 16;
        index = (index + 1) & mask;
        *seed = index;
        dz = (table[index] & bits) - 16;
    }
    *(u16 *)(collider + 0x3E) = damage;
    actor->reactionSoundVariant = damage;
    healthp = &actor->health;
    *healthp -= *(u16 *)(collider + 0x3E);
    flags2D8 = &actor->field2D8;
    if (!(*flags2D8 & 0x20000)) {
        u16 bank = *(u16 *)((u8 *)actor + 0x1E2);
        if (!actor->drawableKind)
            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
        else
            *(void **)((u8 *)actor->drawable + 0x68) = gUnk_030042E4 + bank * 32;
        {
            u32 timer = 8;
            *(u16 *)((u8 *)actor + 0x1E8) = timer;
        }
        {
            u32 mask = 0x8000;
            *flags2CC = mask | *flags2CC;
        }
        if (ShouldShowDamage(actor)) {
            s32 health = *healthp;
            u32 maximum = *(u16 *)((u8 *)actor + 0xB0);
            s32 value = *(s16 *)((u8 *)actor + 0xB2);
            s32 previous = *(s16 *)((u8 *)actor + 0x9A);
            SceneHud_SetTimedState(actor->id, health, maximum, value, 0, previous);
            Fill(&zero, display);
            if (actor->reactionSoundVariant || !(*flags2D8 & 0x20000))
                DamageNumberDisplay_Show(collider, actor->reactionSoundVariant, 0, position,
                                         display, 1);
        }
    }
    SceneActorMovement_ProjectProbe(actor, dx, dz);
    {
        u32 bit = 8;
        actor->flags2D4 = bit | actor->flags2D4;
    }
    *((u8 *)actor + 0x59) = Math_Atan2(dx, dz);
    *timer = 38;
    actor->field2DC = 0x40000;
    *flags2D8 = 0;
    {
        u16 *f = &actor->flags2D4;
        u32 mask = ~0x2000;
        register u32 v asm("r1");
        u32 z;
        asm("" : "+r"(mask));
        v = mask & *f;
        z = 0;
        *f = v;
        actor->field2B8 = z;
    }

    if (*healthp <= 0) {
        *(u16 *)(collider + 0x44) = 0;
        Hide(actor);
        {
            void *control = *(void **)((u8 *)actor + 0x234);
            Reaction_SetControl(actor, 17, control);
        }
        {
            u32 state = 3;
            actor->state2A8 = state;
        }
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        actor->field2E0 = 32;
        if (!Test(gUnk_030053F4, 0x4800) && !Flag2D0(actor, 0x800))
            SceneActorRecord_RecordPair(gUnk_030039F0, actor->id);
    } else {
        {
            u32 bit = 0x20;
            asm("" : "+r"(bit));
            actor->flags2D4 = bit | actor->flags2D4;
        }
        {
            void *control = *(void **)((u8 *)actor + 0x230);
            Reaction_SetControl(actor, 18, control);
        }
        {
            u32 state = 3;
            actor->state2A8 = state;
        }
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
    {
        u32 bit = 0x2000;
        *flags2CC = bit | *flags2CC;
    }
    return 1;
fail:
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08132904);
#endif
