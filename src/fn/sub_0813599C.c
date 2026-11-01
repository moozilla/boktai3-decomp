// MATCHING_CONCESSION: register-pinning
#include "global.h"
/* SceneActorReaction_ApplyEscalatingHit: for category-1 actors flagged 4 at +0x2DC, applies a hit whose damage doubles per stored hit count (+0x47E), shows HUD/damage numbers, restarts the route and flashes the palette; returns 1 when applied (provisional) */
#include "scene_actor_reset.h"
#include "area_effect_link.h"
#include "render.h"
#include "scene_actor_damage_visibility.h"
void SceneActorEffects_EmitRandomOffsets(void *, s32, u32, u32, s32, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneBody_StartRoute(void *, void *, int, int, int);
void AnimData_SelectPaletteBank(void *, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline u8 Pick(u8 *nav)
{
    u8 *choices = nav + 0x81;
    u32 i = 1 ^ nav[0x83];
    return choices[i];
}
struct NavRoute {
    u8 unknown0[0x81];
    u8 choices[2];
    u8 current;
};
static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
u8 SceneActorReaction_ApplyEscalatingHit(struct SceneActorRegistryResetView *actor)
{
    u8 *nav = (u8 *)actor + 0x3D4;
    if (actor->category == 1 && (actor->field2DC & 4)) {
        u32 test = 0x2000;
        u16 *flags2D4 = &actor->flags2D4;
        u32 value = *flags2D4;
        if (!(test & value)) {
            u32 *flags2D0;
            u8 *hits;
            {
                u32 v = 0x10 | value;
                v &= ~0xA007;
                *flags2D4 = v;
            }
            flags2D0 = &actor->flags2D0;
            {
                u32 mask = ~2;
                *flags2D0 &= mask;
            }
            asm("" : "+r"(flags2D0));
            {
                u16 *flags = &actor->flags2D6;
                s32 mask = ~0x9CB;
                *flags &= mask;
            }
            {
                u32 mask = ~0x4000;
                *flags2D0 &= mask;
            }
            hits = (u8 *)actor + 0x47E;
            if (*hits <= 3) {
                u16 *count;
                SceneActorEffects_EmitRandomOffsets(actor, 1, 1, 10, 20, -1);
                count = &actor->group.field5C0;
                {
                    u32 idx = *count * 2;
                    u8 *arr = (u8 *)actor + 0x5B8;
                    {
                        u16 *slot = (u16 *)(arr + idx);
                        u32 v = 0x244;
                        *slot = v;
                    }
                }
                actor->group.field5C2 |= 1 << *count;
                (*count)++;
            }
            actor->reactionSoundVariant = 1 << *hits;
            {
                u32 d = actor->reactionSoundVariant;
                actor->health -= d;
            }
            if (SceneActorDamage_ShouldShow(actor)) {
                u32 zero;
                u32 buffer[2];
                {
                    s32 h = actor->health;
                    s32 maxh = FIELD(actor, 0xB0, u16);
                    s32 kind = FIELD(actor, 0xB2, s16);
                    s32 param = FIELD(actor, 0x9A, s16);
                    SceneHud_SetTimedState(FIELD(actor, 0x50, u32), h, maxh, kind, 1, param);
                }
                Fill(&zero, buffer);
                if (actor->reactionSoundVariant || !(actor->field2D8 & 0x20000))
                    DamageNumberDisplay_Show(&actor->colliderA, actor->reactionSoundVariant, 0,
                                             &actor->body.position, buffer, 1);
            }
            SceneBody_StartRoute(
                (u8 *)actor + 0x50, nav + 0x74,
                ((struct NavRoute *)nav)->choices[1 ^ ((struct NavRoute *)nav)->current], 0, 0);
            {
                u16 *t = (u16 *)(nav + 0x1F0);
                u32 v = 600;
                *t = v;
            }
            {
                u16 bank = FIELD(actor, 0x1E4, u16);
                if (actor->drawableKind == 0)
                    AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
                else
                    FIELD(actor->drawable, 0x68, u8 *) = gPaletteBankData + bank * 32;
            }
            {
                u32 bit = 0x2000000;
                actor->flags2CC |= bit;
            }
            {
                u16 timer = 6000;
                actor->field1EA = timer;
            }
            Sound_PlaySong(0x121);
            return 1;
        }
    }
    return 0;
}
