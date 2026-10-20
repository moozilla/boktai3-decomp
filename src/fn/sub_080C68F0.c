// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define Golem_UpdateChargeStop                                                                     \
    sub_080C68F0 // confidence=low: neighboring Golem controls establish the state role.
#ifdef NONMATCHING
// fuzzy: 81.6%
// Remaining differences: animation selector register allocation and flag-clear order.
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
// confidence=low: charge and slide behaviors install this completion control.
void Golem_UpdateChargeStop(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorAnimationView *animation;
    if (TakeFlag(&actor->field2B2)) {
        {
            u8 directionVariant;
            u8 mirror;
            u8 *mirrorPointer;
            register u8 *variantPointer asm("r4");
            u32 flags;
            u32 variant;
            u32 selection;
            u8 *variantReload;
            u32 force;
            register u8 *restart asm("r8");
            variantPointer = &directionVariant;
            asm("" : "+r"(variantPointer));
            {
                u8 *mirrorAddress = &mirror;
                s32 quadrant = ((((s32)actor->direction) + 0x60) >> 6) & 3;
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
                u32 flagMask = 4;
                selection = 7;
                variant = *variantReload;
                flags = mirrorValue;
                flags |= flagMask;
            }
            if (flags & 0x10) {
                force = 0x10;
                flags &= ~0x10;
            } else {
                force = 0;
            }
            animation = (struct SceneActorAnimationView *)actor;
            if (!animation->drawableKind) {
                struct SceneActorAnimationSequenceDrawable *drawable;
                animation->selection = selection;
                drawable = animation->drawable;
                Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                                 (u8)variant, (u8)flags);
                restart = &animation->restart;
            } else {
                struct SceneActorAnimationTile *tile =
                    &((struct SceneActorAnimationTileDrawable *)animation->drawable)->tile;
                u8 *savedSelection;
                savedSelection = &animation->selection;
                selection += variant;
                restart = &animation->restart;
                if (((!force) && ((*savedSelection) == selection)) && (!(*restart))) {
                    if ((flags == 1) && (tile->flags & 4)) {
                        goto done;
                    }
                    if ((flags == 0) && (!(tile->flags & 4))) {
                        goto done;
                    }
                }
                *savedSelection = selection;
                {
                    register u32 mask asm("r9") = 4;
                    u32 mode;
                    if (flags & mask) {
                        mode = 6;
                    } else {
                        mode = 4;
                    }
                    Anim_SetKeyframes(
                        &((struct SceneActorAnimationTileDrawable *)animation->drawable)->tile,
                        animation->drawable, (u16)selection, 2, (u16)mode);
                    flags &= 1;
                    if (flags) {
                        tile->flags |= mask;
                    } else {
                        tile->flags &= ~4;
                    }
                }
            }
            *restart = 0;
        }
    done:
        actor->field2B3 = 0;
    }
    if (actor->field2B3) {
        {
            u16 *field = &actor->flags2D4;
            u32 mask = ~0x100;
            u32 value = *field;
            mask &= value;
            *field = mask;
        }
        actor->field2B0 = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080C68F0);
#endif
