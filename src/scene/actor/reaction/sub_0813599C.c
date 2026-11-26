#include "global.h"
/* SceneActorReaction_ApplyEscalatingHit: for category-1 actors flagged 4 at +0x2DC, applies a hit whose damage doubles per stored hit count (+0x47E), shows HUD/damage numbers, restarts the route and flashes the palette; returns 1 when applied (provisional) */
#include "scene/actor/scene_actor_reset.h"
#include "scene/actor/scene_actor_chase_types.h"
#include "scene/area_effect/area_effect_link.h"
#include "engine/video/render.h"
#include "scene/actor/scene_actor_damage_visibility.h"
void SceneActorEffects_EmitRandomOffsets(void *, s32, u32, u32, s32, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneBody_StartRoute(void *, void *, int, int, int);
void AnimData_SelectPaletteBank(void *, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

u8 SceneActorReaction_ApplyEscalatingHit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (actor->base.category == 1 && (actor->base.hitReactionFlags & 4)) {
        u32 test = 0x2000;
        u16 *flags2D4 = &actor->base.motionFlags;
        u32 value = *flags2D4;
        if (!(test & value)) {
            u32 *flags2D0;
            u8 *hits;
            {
                u32 v = 0x10 | value;
                v &= ~0xA007;
                *flags2D4 = v;
            }
            flags2D0 = &actor->base.statusFlags;
            {
                u32 mask = ~2;
                *flags2D0 &= mask;
            }
            {
                struct SceneActorChaseFlags16 *flags = (void *)&actor->base.reactionFlags;
                s32 mask = ~0x9CB;
                flags->value &= mask;
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
                    u8 *arr = (u8 *)&actor->group.motion5B8;
                    {
                        u16 *slot = (u16 *)(arr + idx);
                        u32 v = 0x244;
                        *slot = v;
                    }
                }
                actor->group.field5C2 |= 1 << *count;
                (*count)++;
            }
            actor->base.reactionSoundVariant = 1 << *hits;
            {
                u32 d = actor->base.reactionSoundVariant;
                actor->base.health -= d;
            }
            if (SceneActorDamage_ShouldShow(actor)) {
                struct ContactPosition offset;
                {
                    s32 h = actor->base.health;
                    s32 maxh = (u16)actor->base.defaultHealth;
                    s32 kind = (s16)actor->base.hudKind;
                    s32 param = (s16)actor->base.hudLabel;
                    SceneHud_SetTimedState(actor->base.body.id, h, maxh, kind, 1, param);
                }
                ZeroFill8(&offset);
                if (actor->base.reactionSoundVariant || !(actor->base.hitAttributes & 0x20000))
                    DamageNumberDisplay_Show(&actor->base.colliderA,
                                             actor->base.reactionSoundVariant, 0,
                                             &actor->base.body.position, &offset, 1);
            }
            SceneBody_StartRoute(
                &actor->base.body, &group->homeRoute,
                group->homeRoute.branches[1 ^ group->homeRoute.activeBranch], 0, 0);
            {
                group->field5C4 = 600;
            }
            {
                u16 bank = actor->base.alternateOffset;
                if (actor->base.drawableKind == 0)
                    AnimData_SelectPaletteBank((u8 *)actor->base.drawable + 0x2C, bank);
                else
                    FIELD(actor->base.drawable, 0x68, u8 *) = gPaletteBankData + bank * 32;
            }
            {
                u32 bit = 0x2000000;
                actor->base.behaviorFlags |= bit;
            }
            {
                u16 timer = 6000;
                actor->base.paletteTimer = timer;
            }
            Sound_PlaySong(0x121);
            return 1;
        }
    }
    return 0;
}
