// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_object.h"
extern const u16 gUnk_08606130[];
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
struct GolemRouteMotion {
    u8 unknown[0x14];
    s16 x, y, z;
};
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
// confidence=low: return-home behaviors use this control while moving to a cell center.
void Golem_UpdateRouteMotion(struct SceneActorRegistryResetView *actor, u32 frame)
{
    u8 *body = actor->context;
    struct GolemRouteMotion *motion = (void *)((u8 *)actor + 0x50);
    u16 *flags2D4;
    u16 *initialFlags;
    register u32 animationZero asm("r6");
    if (TakeFlag(&actor->field2B2))
        actor->field2B3 = 0;
    {
        u32 mask = 0x100;
        u16 *flags = &actor->flags2D4;
        register u32 value asm("r1") = *flags;
        initialFlags = flags;
        animationZero = 0;
        *flags = mask | value;
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
            flags2D4 = initialFlags;
            if (quadrant > 1) {
                *mirrorPointer = 1;
                *variantPointer = 3 - quadrant;
            } else {
                *mirrorPointer = animationZero;
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
done:
    actor->field2C2 = 0x40 >> (*(s16 *)(body + 0x682) >> 7);
    asm("" : "+r"(actor));
    {
        u32 mask = 0xA02F;
        u16 *scale;
        if (*flags2D4 & mask) {
            actor->field2C2 = 0x40;
            scale = &actor->field2C2;
        } else {
            u16 *p = &actor->field2C2;
            u16 value = *p;
            scale = p;
            if (value == 0)
                *scale = 0x40;
        }
        if (actor->drawableKind == 0)
            Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
        else
            Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
    }

    if (!(frame & 3)) {
        struct SceneExtent velocity, size;
        velocity.x = -motion->x;
        velocity.y = 0;
        velocity.z = -motion->z;
        size.x = 32;
        size.y = 0;
        size.z = 32;
        SceneParticleBurst_CreateWithPause((void *)((u8 *)motion + 0xC), (s32)&velocity, &size, 1,
                                           3, 4);
    }
}
