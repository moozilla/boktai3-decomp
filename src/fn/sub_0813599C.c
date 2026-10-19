#include "global.h"
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
#ifdef NONMATCHING
// fuzzy: 80.9%
// Category-1 hit reaction (damage 1 << hits, HUD timed state, damage number, route restart).
// Control flow right; left: register choice around the flags2D0/2D6 clears and the
// SceneHud_SetTimedState argument order (ROM derives 0xB2/0x9A from the 0xB0 address).
void SceneActorEffects_EmitRandomOffsets(void *, s32, u32, s32, s32, u32);
void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneBody_StartRoute(void *, void *, int, int, int);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline s32 LocalPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}
static inline u8 ShowsDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gUnk_030053F4, 0x4800)) {
        u32 *owner = &actor->field2E0;
        u32 value = *owner;
        if (!(value & 0x20) && value) {
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
struct DamageView {
    u8 unknown0[0x50];
    u32 id;
    u8 unknown54[0x46];
    s16 f9A;
    s16 health;
    u8 unknown9E[0x12];
    u16 fB0;
    s16 fB2;
};
struct HitHistory {
    u8 unknown0[0x5B8];
    u16 kinds[4];
    u16 count;
    u16 mask;
};
u32 sub_0813599C(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    register u32 category asm("r4") = actor->category;
    register u8 *count asm("r6");
    struct {
        u32 zero;
        u32 size[2];
    } fill;
    if (category == 1 && (actor->field2DC & 4)) {
        u32 mask = 0x2000;
        u16 *flags2D4 = &actor->flags2D4;
        u32 flags = *flags2D4;
        if (!(mask & flags)) {
            register u32 *flags2D0 asm("r2");
            {
                u32 value = 0x10;
                value |= flags;
                value &= ~0xA007;
                *flags2D4 = value;
            }
            {
                register u32 offset asm("r6") = 0x2D0;
                u32 clear = ~2;
                asm("" : "+r"(offset));
                flags2D0 = (u32 *)((u8 *)actor + offset);
                asm("" : "+r"(offset));
                *flags2D0 &= clear;
            }
            {
                u16 *flags2D6 = &actor->flags2D6;
                u32 clear = ~0x9CB;
                *flags2D6 = clear & *flags2D6;
                asm("" ::: "memory");
            }
            {
                u32 clear = ~0x4000;
                *flags2D0 &= clear;
            }
            count = (u8 *)actor + 0x47E;
            if (*count <= 3) {
                struct HitHistory *history = (void *)actor;
                SceneActorEffects_EmitRandomOffsets(actor, 1, 1, 10, 20, -1);
                history->kinds[history->count] = 0x244;
                history->mask |= 1 << history->count;
                history->count++;
            }
            category <<= *count;
            {
                u16 *damage = &actor->reactionSoundVariant;
                u32 amount;
                *damage = category;
                asm("" ::: "memory");
                amount = *damage;
                actor->health -= amount;
            }
            if (ShowsDamage(actor)) {
                u32 *buffer;
                SceneHud_SetTimedState(actor->id, actor->health, *(u16 *)((u8 *)actor + 0xB0),
                                       *(s16 *)((u8 *)actor + 0xB2), 1,
                                       *(s16 *)((u8 *)actor + 0x9A));
                buffer = fill.size;
                fill.zero = 0;
                CpuSet(&fill.zero, buffer, 0x05000002);
                if (actor->reactionSoundVariant || !(actor->field2D8 & 0x20000))
                    DamageNumberDisplay_Show(&actor->colliderA, actor->reactionSoundVariant, 0,
                                             &actor->position, buffer, 1);
            }
            SceneBody_StartRoute(&actor->id, group + 0x74, (group + 0x81)[1 ^ group[0x83]], 0, 0);
            *(u16 *)(group + 0x1F0) = 600;
            {
                u32 bank = *(u16 *)((u8 *)actor + 0x1E4);
                if (!actor->drawableKind)
                    AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
                else
                    *(u8 **)((u8 *)actor->drawable + 0x68) = gUnk_030042E4 + bank * 32;
            }
            {
                u32 mask = 0x2000000;
                actor->flags2CC |= mask;
            }
            actor->field1EA = 6000;
            Sound_PlaySong(0x121);
            return 1;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0813599C);
#endif
