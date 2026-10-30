// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
#include "scene_object.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
extern const u16 gUnk_08606130[];
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline s32 Scale8(s16 n)
{
    return n >= 0 ? n >> 8 : -((-n) >> 8);
}
/* Charge windup: reset the pending-hit count, emit dust, and ramp the stored speed. */
// confidence=low: charge controls 080CDF14 and 080CEDF0 install this preparatory state.
void Golem_UpdateChargeWindup(struct SceneActorRegistryResetView *actor, u32 frame)
{
    u8 *body = actor->context;
    u8 *motion = (u8 *)actor + 0x50;
    u16 *flags2D4;
    u16 *initialFlags;
    u16 *scale;
    s16 *speed;
    if (TakeFlag(&actor->field2B2)) {
        actor->field2B3 = 0;
        *(u16 *)(body + 0x634) = 0;
    }
    {
        u8 directionVariant, mirror;
        u8 *mirrorPointer;
        u8 *variantPointer;
        u32 flags, variant, selection;
        u8 *variantReload;
        u32 force;
        u8 *restart;

        variantPointer = &directionVariant;
        {
            u8 *mirrorAddress = &mirror;
            s32 quadrant = (((s32)actor->body.direction + 0x60) >> 6) & 3;
            mirrorPointer = mirrorAddress;
            if (quadrant > 1) {
                *mirrorPointer = 1;
                *variantPointer = 3 - quadrant;
            } else {
                *mirrorPointer = 0;
                *variantPointer = quadrant;
            }
        }
        variantReload = &directionVariant;
        {
            u32 mirrorValue = *mirrorPointer;
            flags = gUnk_08606130[mirrorValue];
            selection = 8;
            variant = *variantReload;
            flags |= mirrorValue;
        }
        if (flags & 0x10) {
            force = 0x10;
            flags &= ~0x10;
        } else
            force = 0;
        if (!((struct SceneActorAnimationView *)actor)->drawableKind) {
            struct SceneActorAnimationSequenceDrawable *drawable;
            ((struct SceneActorAnimationView *)actor)->selection = selection;
            drawable = ((struct SceneActorAnimationView *)actor)->drawable;
            Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                             (u8)variant, (u8)flags);
            restart = &((struct SceneActorAnimationView *)actor)->restart;
        } else {
            struct SceneActorAnimationTile *tile =
                &((struct SceneActorAnimationTileDrawable *)((struct SceneActorAnimationView *)
                                                                 actor)
                      ->drawable)
                     ->tile;
            u8 *savedSelection;

            selection += variant;
            savedSelection = &((struct SceneActorAnimationView *)actor)->selection;
            restart = &((struct SceneActorAnimationView *)actor)->restart;
            if (!force && *savedSelection == selection && !*restart) {
                if (flags == 1 && (tile->flags & 4))
                    goto done;
                if (flags == 0 && !(tile->flags & 4))
                    goto done;
            }
            *savedSelection = selection;
            {
                u32 mask = 4;
                u32 mode;
                if (flags & mask)
                    mode = 6;
                else
                    mode = 4;
                Anim_SetKeyframes(
                    &((struct SceneActorAnimationTileDrawable *)((struct SceneActorAnimationView *)
                                                                     actor)
                          ->drawable)
                         ->tile,
                    ((struct SceneActorAnimationView *)actor)->drawable, (u16)selection, 2,
                    (u16)mode);
                flags &= 1;
                if (flags)
                    tile->flags |= mask;
                else
                    tile->flags &= ~4;
            }
        }
        *restart = 0;
    }
done: {
    u32 mask = 0x20;
    mask |= actor->flags2D6;
    actor->flags2D6 = mask;
}
    {
        u32 mask = 0x100;
        register u32 value asm("r1");
        initialFlags = &actor->flags2D4;
        value = *initialFlags;
        *initialFlags = mask | value;
    }
    frame &= 3;
    flags2D4 = initialFlags;
    if (frame == 0) {
        struct SceneExtent velocity, size;
        u32 direction;
        u32 count;
        /* Retain the original RNG scratch lifetime; this literal is never dereferenced. */
        register u32 unused asm("r4") = 0x0403B714;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 mask = 0xFF;
            direction = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex] & mask;
        }
        velocity.x = Scale8(gUnk_086149C4[(direction + 0x40) & 0xFF]);
        velocity.y = 0;
        velocity.z = Scale8(gUnk_086149C4[(direction + 0x40) & 0xFF]);
        asm("" : : "r"(unused));
        size.x = 48;
        size.y = 0;
        size.z = 48;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 bits = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
            bits &= 1;
            count = bits + 2;
        }
        SceneParticleBurst_CreateWithPause((void *)(motion + 0xC), (s32)&velocity, &size, 2, count,
                                           4);
    }
    speed = (s16 *)(body + 0x682);
    if (*speed <= 383)
        *speed += 16;
    actor->field2C2 = 0x40 >> (*speed >> 7);
    asm("" : "+r"(actor));
    {
        u32 mask = 0xA02F;
        u16 *flagCopy = flags2D4;
        u32 masked = *flagCopy & mask;
        if (masked) {
            actor->field2C2 = 0x40;
            scale = &actor->field2C2;
        } else {
            u16 *p = &actor->field2C2;
            u16 value = *p;
            scale = p;
            if (!value)
                *scale = 0x40;
        }
        if (actor->drawableKind == 0)
            Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
        else
            Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
    }
}
