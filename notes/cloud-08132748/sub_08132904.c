#include "global.h"
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneActorRecord_RecordPair(u16, u16);
s32 AreaEffect_ScaleLinkGainAtPosition(u8 *, s32);
s32 AreaEffect_ScaleGainAtPosition(u8 *, s32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
extern u16 gUnk_030039F0;
extern u32 gUnk_03005308;
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
static inline s32 RandomOffset(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return (*(const u16 *)((u32)gUnk_0203B400 + gUnk_03005308 * 2) & 63) - 16;
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
static inline u32 Flag2D0(struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2D0 & mask;
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
u32 sub_08132904(struct SceneActorRegistryResetView *actor)
{
    u32 *flags2CC;
    u8 *collider;
    u16 *timer;
    s32 gain;
    s32 dx, dz;
    u16 *flags2D4;
    s16 *health;
    struct {
        u32 zero;
        u32 size[2];
    } fill;
    if (!Flag2D0(actor, 8) && actor->enabled == 1) {
        u32 two = 2;
        flags2CC = &actor->flags2CC;
        if (!(*flags2CC & two)) {
            collider = (u8 *)&actor->colliderA;
            timer = (u16 *)((u8 *)actor + 0x178);
            if (!*timer) {
                if (Test(gUnk_030053F4, 0x4800))
                    gain = AreaEffect_ScaleLinkGainAtPosition((u8 *)&actor->position,
                                                              *(u16 *)((u8 *)actor + 0xB0));
                else
                    gain = AreaEffect_ScaleGainAtPosition((u8 *)&actor->position,
                                                          *(u16 *)((u8 *)actor + 0xB0));
                if (gain) {
                    {
                        u32 index = (gUnk_03005308 + 1) & 1023;
                        dx = (gUnk_0203B400[index] & 63) - 16;
                        index = (index + 1) & 1023;
                        gUnk_03005308 = index;
                        dz = (gUnk_0203B400[index] & 63) - 16;
                    }
                    *(u16 *)(collider + 0x3E) = gain;
                    actor->reactionSoundVariant = gain;
                    health = &actor->health;
                    *health -= *(u16 *)(collider + 0x3E);
                    if (!(actor->field2D8 & 0x20000)) {
                        u32 bank = *(u16 *)((u8 *)actor + 0x1E2);
                        if (!actor->drawableKind)
                            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
                        else
                            *(u8 **)((u8 *)actor->drawable + 0x68) = gUnk_030042E4 + bank * 32;
                        actor->field1E8 = 8;
                        *flags2CC |= 0x8000;
                        if (ShowsDamage(actor)) {
                            u32 *buffer;
                            SceneHud_SetTimedState(actor->id, *health, *(u16 *)((u8 *)actor + 0xB0),
                                                   *(s16 *)((u8 *)actor + 0xB2), 0,
                                                   *(s16 *)((u8 *)actor + 0x9A));
                            buffer = fill.size;
                            fill.zero = 0;
                            CpuSet(&fill.zero, buffer, 0x05000002);
                            if (actor->reactionSoundVariant || !(actor->field2D8 & 0x20000))
                                DamageNumberDisplay_Show(collider, actor->reactionSoundVariant, 0,
                                                         &actor->position, buffer, 1);
                        }
                    }
                    SceneActorMovement_ProjectProbe(actor, dx, dz);
                    flags2D4 = &actor->flags2D4;
                    *flags2D4 |= 8;
                    actor->direction = Math_Atan2(dx, dz);
                    *timer = 38;
                    actor->field2DC = 0x40000;
                    actor->field2D8 = 0;
                    *flags2D4 &= ~0x2000;
                    actor->field2B8 = 0;
                    if (*health <= 0) {
                        *(u16 *)(collider + 0x44) = 0;
                        Hide(actor);
                        {
                            void *control = *(void **)((u8 *)actor + 0x234);
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
                        if (!Test(gUnk_030053F4, 0x4800) && !(actor->flags2D0 & 0x800))
                            SceneActorRecord_RecordPair(gUnk_030039F0, actor->id);
                    } else {
                        *flags2D4 |= 0x20;
                        {
                            void *control = actor->reactionTarget230;
                            Reaction_SetControl(actor, 18, control);
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
                    *flags2CC |= 0x2000;
                    return 1;
                }
            }
        }
    }
    return 0;
}
