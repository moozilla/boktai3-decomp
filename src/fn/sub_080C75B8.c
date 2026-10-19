#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.8%
// Recoil state: plays sequence 8 (same animation inline as SceneActorReaction_SlideBack),
// decays the context speed by the category step, slides back toward the saved position along
// the facing axis, then waits 8 frames. 13 differing instructions ignoring registers: where
// the zero constants are materialised, and the ROM reloads field2C2 before testing it.
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"

extern const u16 gUnk_08606130[];
void sub_080C47B8(struct ContactPosition *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

struct Unk080C75B8Motion {
    u8 unknown0[0xC];
    struct ContactPosition position;
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

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

void sub_080C75B8(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    struct Unk080C75B8Motion *motion = (struct Unk080C75B8Motion *)((u8 *)actor + 0x50);
    s16 *speed;
    u16 *flags2D4;

    if (TakeFlag(&actor->field2B2)) {
        actor->field2B3 = 0;
        body[0x68D] = 0;
        {
            u32 quadrant = ((body[0x68E] + 0x60) >> 6) & 3;
            body[0x68C] = quadrant;
        }
        *(struct ContactPosition *)(body + 0x69C) = motion->position;
        sub_080C47B8((struct ContactPosition *)(body + 0x69C));
    }
    {
        u32 mask = 0x100;
        flags2D4 = &actor->flags2D4;
        *flags2D4 |= mask;
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
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
done:
    if (body[0x68D] == 0) {
        switch (*(u16 *)((u8 *)actor + 0x1E6)) {
            case 1:
            case 3:
            case 5:
                *(s16 *)(body + 0x682) -= 0xC0;
                break;
            case 0:
                *(s16 *)(body + 0x682) -= 0x40;
                break;
            default:
                *(s16 *)(body + 0x682) -= 0x80;
                break;
        }
        speed = (s16 *)(body + 0x682);
        if (*speed < 0) {
            body[0x68D] = 1;
            *speed = 0;
            return;
        }
        actor->motion.x += (*speed * gUnk_086149C4[(body[0x68E] + 0x40) & 0xFF]) >> 16;
        actor->motion.z += (*speed * gUnk_086149C4[body[0x68E]]) >> 16;
    } else if (body[0x68D] == 1) {
        struct ContactPosition delta;
        Delta(&delta, &motion->position, (struct ContactPosition *)(body + 0x69C));
        switch (body[0x691] >> 6) {
            case 0:
            case 2:
                delta.z = 0;
                if (delta.x > 8)
                    motion->x = 8;
                else if (delta.x < -8)
                    motion->x = -8;
                else
                    motion->x = delta.x;
                break;
            case 1:
            case 3:
                delta.x = 0;
                if (delta.z > 8)
                    motion->z = 8;
                else if (delta.z < -8)
                    motion->z = -8;
                else
                    motion->z = delta.z;
                break;
        }
        if (delta.x + delta.z == 0) {
            body[0x68D] = 2;
            body[0x68F] = 0;
            return;
        }
    } else {
        if (++body[0x68F] > 7) {
            actor->field2B0 = 1;
            body[0x68F] = 0;
            *(u16 *)(body + 0x682) = 0;
            return;
        }
    }
    actor->field2C2 = 0x40 >> (*(u16 *)(body + 0x684) >> 7);
    {
        u32 mask = 0xA02F;
        u16 *scale;
        if (*flags2D4 & mask) {
            actor->field2C2 = 0x40;
            scale = &actor->field2C2;
        } else {
            scale = &actor->field2C2;
            if (*scale == 0)
                *scale = 0x40;
        }
        if (actor->drawableKind == 0)
            Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
        else
            Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C75B8);
#endif
