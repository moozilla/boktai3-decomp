#include "global.h"
/* SceneActorReaction_SlideBack: on entry plays sequence 8 facing the context direction (+0x691); slides along the dominant axis of that direction at the context speed (+0x682), decaying it by 0x10 per frame, and flags completion (+0x2B0) 12 frames after it stops (provisional name) */
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"

extern const u16 gUnk_08606130[];

struct Unk080C79A8Motion {
    u8 unknown0[0x14];
    s16 x, unknown16, z;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
static inline s32 Scale16(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

void SceneActorReaction_SlideBack(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    struct Unk080C79A8Motion *motion = (struct Unk080C79A8Motion *)((u8 *)actor + 0x50);
    s32 dx, dz;
    s16 *speed;

    if (TakeFlag(&actor->field2B2)) {
        u8 *direction = &((struct SceneActorAnimationView *)actor)->direction;
        u8 saved = *direction;
        u8 directionVariant, mirror;
        u8 *mirrorPointer;
        u8 *variantPointer;
        u32 flags, variant, selection;
        u8 *variantReload;
        u32 force;
        u8 *restart;

        *direction = body[0x691];
        variantPointer = &directionVariant;
        {
            u8 *mirrorAddress = &mirror;
            s32 quadrant = (((s32)*direction + 0x60) >> 6) & 3;
            mirrorPointer = mirrorAddress;
            if (quadrant > 1) {
                *mirrorPointer = 1;
                *variantPointer = 3 - quadrant;
            } else {
                *mirrorPointer = 0;
                *variantPointer = quadrant;
            }
        }
        *(&((struct SceneActorAnimationView *)actor)->direction) = saved;
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
        } else {
            force = 0;
        }
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
                if ((flags == 1) && (tile->flags & 4)) {
                    goto done;
                }
                if ((flags == 0) && (!(tile->flags & 4))) {
                    goto done;
                }
            }
            *savedSelection = selection;
            {
                u32 mask = 4;
                u32 mode;
                if (flags & mask) {
                    mode = 6;
                } else {
                    mode = 4;
                }
                Anim_SetKeyframes(
                    &((struct SceneActorAnimationTileDrawable *)((struct SceneActorAnimationView *)
                                                                     actor)
                          ->drawable)
                         ->tile,
                    ((struct SceneActorAnimationView *)actor)->drawable, (u16)selection, 2,
                    (u16)mode);
                flags &= 1;
                if (flags) {
                    tile->flags |= mask;
                } else {
                    tile->flags &= ~4;
                }
            }
        }
        *restart = 0;
    done:
        ((struct SceneActorAnimationView *)actor)->restart = 0;
        body[0x68F] = 0;
    }
    {
        u32 mask = 0x100;
        actor->flags2D4 |= mask;
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    dx = Scale16(gUnk_086149C4[(body[0x691] + 0x40) & 0xFF] * *(s16 *)(body + 0x682));
    dz = Scale16(gUnk_086149C4[body[0x691]] * *(s16 *)(body + 0x682));
    if (((dx < 0) ? (-dx) : (dx)) < ((dz < 0) ? (-dz) : (dz))) {
        motion->z = dz;
    } else {
        motion->x = dx;
    }
    speed = (s16 *)(body + 0x682);
    if (*speed <= 0) {
        s16 zero = 0;
        *speed = zero;
        if (++body[0x68F] > 11) {
            actor->field2B0 = 1;
            *speed = zero;
        }
    } else {
        *speed -= 0x10;
    }
}
