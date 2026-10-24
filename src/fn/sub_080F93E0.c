#include "scene_actor_animation.h"

// confidence=low: turns one octant toward the requested direction on animation ticks.

void Axe_GetDirectionalFrame(u8, u32 *, u32 *);
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

#define FIELD(p, o, t) (*(t *)((u8 *)(p) + (o)))
void Anim_SetDurationScale(void *, u16);
void Anim_SetFrameDurationScale(void *, u16);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void Control(struct SceneActorAnimationView *a, void *callback, u32 phase)
{
    u32 zero = 0;
    a->directionalEntryFlag = 1;
    a->unknown2B0 = zero;
    FIELD(a, 0x2B5, u8) = phase;
    FIELD(a, 0x21C, void *) = callback;
    FIELD(a, 0x2C8, u16) = zero;
    {
        u32 *flags = &FIELD(a, 0x114, u32);
        u32 mask = ~1;
        *flags &= mask;
    }
    FIELD(a, 0x104, u32) = zero;
}
// confidence=low: turns one octant toward the requested direction on animation ticks.
static inline u32 Arg1(u32 k_, struct SceneActorAnimationView *actor)
{
    return (*((u16 *)(((u8 *)actor) + 0x2D4))) & k_;
}

static inline u32 Arg2(u32 k_, struct SceneActorAnimationView *actor)
{
    return (*((u32 *)(((u8 *)actor) + 0x2CC))) & k_;
}

inline static void Control_UpdateTurnAnimation(struct SceneActorAnimationView *a, void *callback,
                                               u32 phase)
{
    a->directionalEntryFlag = 1;
    a->unknown2B0 = 0;
    *((u8 *)(((u8 *)a) + 0x2B5)) = phase;
    *((void **)(((u8 *)a) + 0x21C)) = callback;
    *((u16 *)(((u8 *)a) + 0x2C8)) = 0;
    {
        u32 *flags = &(*((u32 *)(((u8 *)a) + 0x114)));
        u32 mask = ~1;
        *flags &= mask;
    }
    *((u32 *)(((u8 *)a) + 0x104)) = 0;
}

void Axe_UpdateTurnAnimation(struct SceneActorAnimationView *actor, u32 events)
{
    u8 *direction = ((u8 *)actor) + 0x50;
    u8 *sequence;
    u16 *scale;
    if (Take(&actor->directionalEntryFlag)) {
        s32 current = ((actor->direction + 0x70) >> 5) & 7;
        s32 target = ((actor->nextDirection + 0x70) >> 5) & 7;
        if (current == target) {
            actor->direction = actor->nextDirection;
            Control_UpdateTurnAnimation(actor, *((void **)(((u8 *)actor) + 0x22C)), 0);
            actor->unknown2B0 = 1;
            return;
        }
        target -= 8;
        {
            s32 k1 = 4;
            if (((u8)((current - target) & 7)) <= k1) {
                u32 *p = (u32 *)(((u8 *)actor) + 0x2CC);
                u32 mask = ~0x10000;
                *p &= mask;
            } else {
                u32 mask = 0x10000;
                *((u32 *)(((u8 *)actor) + 0x2CC)) |= mask;
            }
        }
    }
    sequence = ((u8 *)actor->drawable) + 0x48;
    if (Arg1(0xA02F, actor)) {
        u16 *p = &(*((u16 *)(((u8 *)actor) + 0x2C2)));
        *p = 0x40;
        scale = p;
    } else {
        u16 *p = &(*((u16 *)(((u8 *)actor) + 0x2C2)));
        u16 v = *p;
        scale = p;
        if (!v)
            *scale = 0x40;
    }
    if (!actor->drawableKind)
        Anim_SetDurationScale(((u8 *)(*((void *volatile *)(((u8 *)actor) + 4)))) + 0x48, *scale);
    else
        Anim_SetFrameDurationScale(((u8 *)(*((void *volatile *)(((u8 *)actor) + 4)))) + 0x20,
                                   *scale);
    if (events & 4) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        u32 variant;
        u32 flags;
        u32 selection;
        Control_UpdateTurnAnimation(actor, *((void **)(((u8 *)actor) + 0x224)), 2);
        if (Arg2(0x10000, actor))
            direction[9] += 32;
        else
            direction[9] -= 32;
        Axe_GetDirectionalFrame(direction[9], &variant, &flags);
        sequence[5] = 0;
        selection = entry->value;
        Select(actor, selection, variant, flags);
    }
}
