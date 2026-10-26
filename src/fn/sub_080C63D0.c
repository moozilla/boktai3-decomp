// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_object.h"
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
struct SceneActorPlacementView;
void SceneActorPlacement_PlayVariantEventSound(struct SceneActorPlacementView *, u32);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
struct GolemChargeContext {
    u8 unknown[0x628];
    u32 flags;
};
static inline u8 TestBody(struct GolemChargeContext *body, u32 mask)
{
    return Test(body->flags, mask);
}
static inline u32 Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}
static inline s32 WaitTime(struct GolemChargeContext *context, u32 mask, s32 shift, s32 base)
{
    u32 bits = mask;
    u32 *flags;
    u8 tested;
    u32 masked;
    u32 *clear;
    flags = &context->flags;
    masked = *flags & bits;
    clear = flags;
    tested = Test(masked, ~0);
    if (tested) {
        *clear &= ~mask;
        return 0;
    }
    return ((Random() & 3) << shift) + base;
}
// confidence=low: ramps speed and waits for a species-dependent windup duration.
void Golem_UpdateChargeBuildup(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u8 *body = actor->context;
    u16 *flags2D4;
    if (TakeFlag(&actor->field2B2)) {
        struct GolemChargeContext *context = actor->context;
        s32 duration;
        switch (*(u16 *)((u8 *)actor + 0x1E6)) {
            case 1:
            case 3:
            case 5:
                duration = WaitTime(context, 0x800, 2, 30);
                break;
            case 0:
                duration = WaitTime(context, 0x800, 4, 50);
                break;
            default:
                duration = 0;
                break;
        }
        *(u16 *)(body + 0x68A) = duration;
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
            s32 quadrant = (((s32)actor->direction + 0x60) >> 6) & 3;
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
    u32 mask = 0x100;
    u16 *flags = &actor->flags2D4;
    register u32 value asm("r1") = *flags;
    flags2D4 = flags;
    *flags = mask | value;
}
    if (*(s16 *)(body + 0x682) <= 0x17F)
        *(s16 *)(body + 0x682) += 16;
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

    if (frame > *(u16 *)(body + 0x68A)) {
        actor->field2B0 = 1;
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
    }
}
