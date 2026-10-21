#include "global.h"
// confidence=low: hidden recovery disables the shadow and waits to enter revival.
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_animation.h"
static inline void SelectRecoveryAnimation(struct SceneActorAnimationView *actor, u32 zero)
{
    u8 directionVariant, mirror;
    u32 selection;
    register u8 *mirrorPointer asm("r2");
    u8 *variantPointer = &directionVariant;
    u32 flags;
    register u32 mirrorValue asm("r2");
    u8 variant;
    register u32 entryFlags asm("r0");
    u8 *variantReload;
    u32 force;
    u8 *restart;
    {
        u8 *mirrorAddress = &mirror;
        s32 quadrant = (((s32)actor->direction + 0x60) >> 6) & 3;
        asm("" : "+r"(quadrant), "+r"(mirrorAddress));
        mirrorPointer = mirrorAddress;
        if (quadrant > 1) {
            *mirrorPointer = 1;
            *variantPointer = 3 - quadrant;
        } else {
            *mirrorPointer = zero;
            *variantPointer = quadrant;
        }
    }
    variantReload = &directionVariant;
    mirrorValue = *mirrorPointer;
    entryFlags = 4;
    selection = 8;
    variant = *variantReload;
    flags = mirrorValue | entryFlags;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        {
            void *sequence = drawable->sequence;
            void *resource = (void *)drawable->resource;
            u32 selected = (u16)selection;
            register u32 row asm("r3");
            row = variant;
            Anim_SetSequence(sequence, resource, selected, row, flags);
        }
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
                goto selected;
            if (flags == 0 && !(tile->flags & 4))
                goto selected;
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
            flags &= 1;
            asm("" : "+r"(flags));
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
selected:;
}

#include "scene_actor_reaction.h"
void Mummy_UpdateRevival(void *);
void Mummy_UpdateHiddenRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    register u32 queued asm("r5");
    u32 t;
    u8 *prepared;
    u32 mask = 1;
    if (!actor->drawableKind) {
        u32 *flags = actor->drawable;
        *flags |= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
        tile->flags |= mask;
    }
    {
        register u32 off asm("r1") = 0x2B2;
        register u8 *q asm("r0");
        register u32 v asm("r1");
        asm("" : "+r"(off));
        q = (u8 *)actor + off;
        asm("" : "+r"(q));
        v = *q;
        prepared = q;
        asm("" : "+r"(v));
        t = v ? ({
            register u32 z asm("r0") = 0;
            register u8 *pp asm("r5");
            asm("" : "+r"(z));
            pp = prepared;
            asm("" : "+r"(pp));
            *pp = z;
            1;
        })
              : 0;
    }
    asm("" : "+r"(t));
    queued = t;
    if (queued) {
        register u32 off asm("r1") = 0x320;
        asm("" : "+r"(off));
        ActorShadow_Disable((u8 *)actor + off);
    } else if (frame > 0x1A) {
        void *callback;
        u32 kind, zero, one;
        SelectRecoveryAnimation((void *)actor, queued);
        callback = Mummy_UpdateRevival;
        kind = 13;
        zero = 0;
        one = 1;
        *prepared = one;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
    } else
        actor->field2B3 = queued;
}
