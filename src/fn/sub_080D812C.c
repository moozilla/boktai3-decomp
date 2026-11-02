#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
#include "render.h"
#include "runtime.h"
void SceneActorRecord_RecordPair(u16, u16);
void Player_NotifyFirstSelectedSlot(u32, u32);
void Player_RefreshLifeState(u32, u32);
void sub_08159164(u32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void Sound_PlaySong(u32);
void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
struct BurnState {
    u8 unknown0[0xAA];
    u8 intensity;
    u8 unknownAB[0x1E4 - 0xAB];
    u16 durations[4];
    u16 count, active, timer;
};
#define STATS   ((u8 *)gGameState)
#define H(p, n) (*(s16 *)((p) + (n)))
struct GroupLink {
    u32 unknown0;
    u32 flags;
};
static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
/* Saturating defeat counters in the game-state block. */
#define STAT_INC(n) Count(&H(STATS, n))
#define STAT_ADD(n) H(STATS, n) = clamp(H(STATS, n) + 1)
#define KILL_KIND(flags)                                                                           \
    {                                                                                              \
        if ((flags) & 0x80)                                                                        \
            STAT_ADD(0x58A);                                                                       \
        else if ((flags) & 0x100)                                                                  \
            STAT_ADD(0x58C);                                                                       \
        else if ((flags) & 0x200)                                                                  \
            STAT_ADD(0x58E);                                                                       \
        else if ((flags) & 0x400)                                                                  \
            STAT_ADD(0x590);                                                                       \
        else if ((flags) &= 0x800)                                                                 \
            STAT_ADD(0x592);                                                                       \
    }
#define CASE_BODY(n)                                                                               \
    {                                                                                              \
        u32 flags = *(u32 *)(hit + 0x38);                                                          \
        STAT_INC(n);                                                                               \
        KILL_KIND(flags);                                                                          \
    }
static inline void Count(s16 *p)
{
    if (*p < 9999)
        *p = *p + 1;
    else
        *p = 9999;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Mask(u16 *flags, u32 maskInput)
{
    u32 mask = maskInput;
    return *flags & mask;
}
static inline u8 Test2(u16 **out, struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    u16 *flags = &actor->flags2C0;
    u32 value = *flags & mask;
    *out = flags;
    if (value)
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
static inline u8 Blocked(struct SceneActorRegistryResetView *s)
{
    if (!s)
        goto zero;
    {
        u32 mask = 0x40;
        if (s->flags2D4 & mask)
            goto one;
    }
    {
        u32 mask = 0x20000;
        if (s->flags2D0 & mask)
            goto one;
    }
    if (s->health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 mask = actor->field2E0;
        if (!(mask & 0x20) && mask) {
            switch (LocalPlayer()) {
                case 0:
                    if (actor->field2E0 != 1)
                        return 0;
                    break;
                case 1:
                    if (actor->field2E0 != 2)
                        return 0;
                    break;
                case 2:
                    if (actor->field2E0 != 4)
                        return 0;
                    break;
                case 3:
                    if (actor->field2E0 != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
static inline s32 CurrentHealth(struct SceneActorRegistryResetView *actor)
{
    s16 *health = (void *)&actor->health;
    return *health;
}
static inline void OrStatus(u16 status, u16 *flags)
{
    *flags |= status;
}
// confidence=low: timer-driven damage and defeat recording; the status is tentatively burn.
#define Mummy_UpdateBurnDamage sub_080D812C
void Mummy_UpdateBurnDamage(struct SceneActorRegistryResetView *actor)
{
    struct BurnState *burn = (void *)&actor->group.common;
    register u16 *flags2C0 asm("r6");
    u8 *category;
    u8 *collider;
    u8 *hit;
    u32 kind;
    s32 i;
    struct ContactCollider *damageCollider;
    u32 *damageFlags;
    if (Blocked(actor))
        return;
    if (!burn->timer)
        return;
    if (actor->field2B5 == 0x15) {
        {
            u32 n = 0x20;
            actor->field1EA = n;
            actor->flags2D4 &= n - 0x31;
        }
        actor->field2C2 = 0x40;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        burn->timer = 0;
        return;
    }
    if (burn->intensity > 2) {
        if (Mod(burn->timer, 16) == 0) {
            Sound_PlaySong(0x121);
            OrStatus(0x10, &actor->flags2D6);
        }
    } else {
        if (Mod(burn->timer, 48) == 0) {
            Sound_PlaySong(0x121);
            OrStatus(0x20, &actor->flags2D6);
        }
    }
    if (burn->count) {
        for (i = 0; i < burn->count; i++) {
            u32 value = burn->durations[i];
            if (!value) {
                SceneActorEffects_AdvanceSteps(actor, 1);
                burn->durations[i] = value;
                burn->active &= ~(1 << i);
                burn->count--;
            }
            burn->durations[i]--;
        }
    }
    burn->timer--;
    if (Mod(burn->timer, 40) == 0) {
        {
            struct ContactCollider *initial = &actor->colliderA;
            u32 *flags;
            u32 masked;
            {
                u32 cost = 1 << burn->intensity;
                initial->cost = cost;
                actor->reactionSoundVariant = cost;
            }
            {
                u32 amount = initial->cost;
                u16 *health;
                health = (void *)&actor->health;
                *health -= amount;
            }
            flags = &actor->field2D8;
            masked = *flags & 0x20000;
            damageCollider = initial;
            damageFlags = flags;
            if (!masked) {
                u16 bank = *(u16 *)((u8 *)actor + 0x1E2);
                {
                    struct SceneActorRegistryResetView *p = actor;
                    if (p->drawableKind == 0)
                        AnimData_SelectPaletteBank((u8 *)p->drawable + 0x2C, bank);
                    else {
                        struct SceneActorRegistryResetView *q = actor;
                        *(u8 **)((u8 *)q->drawable + 0x68) = gPaletteBankData + bank * 32;
                    }
                }
                {
                    u32 n = 8;
                    actor->field1E8 = n;
                }
                {
                    u32 bit = 0x8000;
                    actor->flags2CC |= bit;
                }
                if (ShouldShowDamage(actor)) {
                    u32 zero;
                    u32 buffer[2];
                    s32 health = CurrentHealth(actor);
                    u32 max = *(u16 *)((u8 *)actor + 0xB0);
                    s32 type = *(s16 *)((u8 *)actor + 0xB2);
                    s32 param = *(s16 *)((u8 *)actor + 0x9A);
                    SceneHud_SetTimedState(actor->body.id, health, max, type, 0, param);
                    Fill(&zero, buffer);
                    if (actor->reactionSoundVariant || !(*damageFlags & 0x20000))
                        DamageNumberDisplay_Show(damageCollider, actor->reactionSoundVariant, 0,
                                                 &actor->body.position, buffer, 1);
                }
            }
        }
    }
    if (actor->health > 0)
        return;
    {
        u32 state = 3;
        actor->state2A8 = state;
    }
    actor->phase2A9 = 0;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    {
        void *control = *(void **)((u8 *)actor + 0x234);
        Reaction_SetControl(actor, 17, control);
    }
    if (Test(gEngineFlags, 0x4800)) {
        Player_NotifyFirstSelectedSlot(actor->field2E0, actor->field2D8);
        goto done;
    }
    {
        u8 *address = &actor->category;
        u32 owner;
        kind = *address;
        owner = actor->field2E0 & 1;
        collider = (u8 *)&actor->colliderA;
        category = address;
        flags2C0 = &actor->flags2C0;
        if (!owner)
            goto record;
    }
    {
        u8 blocked = Test(Mask(flags2C0, 0x80), ~0);
        collider = (u8 *)&actor->colliderA;
        if (blocked)
            goto record;
    }
    {
        if (kind != 5 && kind != 7 && kind != 6) {
            STAT_INC(0x87C);
            STAT_INC(0x538);
            {
                u32 offset = 0x134;
                u8 *pinned;
                pinned = (u8 *)actor + offset;
                hit = pinned;
            }
            {
                u32 index = *category - 1; /* category 1..25 */
                collider = hit;
                switch (index) {
                    case 3:
                        CASE_BODY(0X53A);
                        break;
                    case 0:
                        CASE_BODY(0X53C);
                        break;
                    case 1:
                        CASE_BODY(0X53E);
                        break;
                    case 2:
                        CASE_BODY(0X540);
                        break;
                    case 13:
                        CASE_BODY(0X54C);
                        break;
                    case 23:
                        CASE_BODY(0X55C);
                        break;
                    case 9:
                        CASE_BODY(0X546);
                        break;
                    case 4:
                    case 5:
                    case 6:
                        CASE_BODY(0X542);
                        break;
                    case 20:
                        CASE_BODY(0X554);
                        break;
                    case 17:
                        CASE_BODY(0X550);
                        break;
                    case 18:
                        CASE_BODY(0X552);
                        break;
                    case 16:
                        CASE_BODY(0X54E);
                        break;
                    case 21:
                        CASE_BODY(0X556);
                        break;
                    case 19:
                        CASE_BODY(0X558);
                        break;
                    case 8:
                        CASE_BODY(0X544);
                        break;
                    case 22:
                        CASE_BODY(0X55A);
                        break;
                    case 11:
                    case 12:
                        CASE_BODY(0X54A);
                        break;
                    case 24:
                        CASE_BODY(0X560);
                        break;
                }
            }
        }
        sub_08159164(*(u32 *)((u8 *)actor + 0xAC));
        Player_RefreshLifeState(actor->field2D8, *((u8 *)actor + 0xA1));
    }

record:
    if (!Test(Mask(flags2C0, 0x80), ~0) && !Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->body.id);
    ((struct ContactCollider *)collider)->flags |= 4;
done:
    burn->timer = 0;
    actor->field2B8 = 0;
}
