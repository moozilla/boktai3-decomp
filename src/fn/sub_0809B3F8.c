#include "global.h"
// fix.py best version (reprinted C)
// Remaining differences include octant-mask lifetime, control initialization and registers.
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u32 Bits(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}

static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = 0;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = 0;
}

#include "scene_actor_animation.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
void Lun_GetDirectionalFrame(u8, u32 *, u32 *);
s32 Mod(s32, s32);
void Player_SubtractEnergy(void *, u32);
void sub_0813D834(void *, u32, u32);
static inline void Select(struct SceneActorAnimationView *actor, u32 selection, u32 variant,
                          u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->selection;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

// confidence=low: turns one octant at animation events until reaching the requested heading.
void Lun_TurnTowardSavedDirection(struct SceneActorRegistryResetView *actor)
{
    u8 *motion = ((u8 *)actor) + 0x50;
    u8 *sequence;
    if (Take(&actor->field2B2)) {
        s32 current = ((((s32)actor->direction) + 0x70) >> 5) & 7;
        u8 desired = *(((u8 *)actor) + 0x3CD);
        s32 target = ((((s32)desired) + 0x70) >> 5) & 7;
        if (current == target) {
            actor->direction = desired;
            Control(actor, actor->savedTarget, 0);
            actor->field2B0 = 1;
            return;
        }
        target -= 8;
        {
            s32 k1 = 4;
            if (((u8)((current - target) & 7)) <= k1) {
                u32 *p = &actor->flags2CC;
                u32 mask = ~0x10000;
                *p &= mask;
            } else {
                u32 mask = 0x10000;
                actor->flags2CC |= mask;
            }
        }
    }
    sequence = (*((u8 **)(((u8 *)actor) + 4))) + 0x48;
    {
        u32 mask = 0xA02F;
        u16 *scale;
        if (actor->flags2D4 & mask) {
            actor->field2C2 = 64;
            scale = &actor->field2C2;
        } else {
            u16 *p = &actor->field2C2;
            u16 value = *p;
            scale = p;
            if (!value)
                *scale = 64;
        }
        if (!(*(((u8 *)actor) + 8)))
            Anim_SetDurationScale((*((u8 **)(((u8 *)actor) + 4))) + 0x48, *scale);
        else
            Anim_SetFrameDurationScale((*((u8 **)(((u8 *)actor) + 4))) + 0x20, *scale);
    }
    if (actor->field2CA & 4) {
        u32 variant;
        u32 flags;
        u32 selection;
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        Control(actor, actor->entryTarget224, 2);
        if (Bits(actor, 0x10000))
            motion[9] += 32;
        else
            motion[9] -= 32;
        Lun_GetDirectionalFrame(motion[9], &variant, &flags);
        sequence[5] = 0;
        selection = entry->value;
        Select((void *)actor, selection, variant, flags);
    }
}
