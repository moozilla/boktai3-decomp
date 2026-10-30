// confidence=low: existing Golem state interpretation retained.
// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_object.h"
extern const u16 gUnk_08606130[];
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
#define A(a) ((struct SceneActorAnimationView *)(a))
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
    if (TakeFlag(&actor->field2B2)) {
        {
            u8 directionVariant;
            u8 mirror;
            register u8 *mirrorPointer asm("r2");
            u8 *variantPointer;
            u32 flags;
            u32 variant;
            u32 selection;
            u8 *variantReload;
            u32 force;
            u8 *restart;
            variantPointer = &directionVariant;
            {
                u8 *mirrorAddress = &mirror;
                s32 quadrant = ((((s32)actor->body.direction) + 0x60) >> 6) & 3;
                asm("" : "+r"(quadrant), "+r"(mirrorAddress));
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
                register u32 mirrorValue asm("r2") = *mirrorPointer;
                u32 flagMask = 4;
                asm("" : "+r"(mirrorValue), "+r"(flagMask));
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
            if (!A(actor)->drawableKind) {
                struct SceneActorAnimationSequenceDrawable *drawable;
                A(actor)->selection = selection;
                drawable = A(actor)->drawable;
                Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                                 (u8)variant, flags);
                restart = &A(actor)->restart;
            } else {
                struct SceneActorAnimationTile *tile =
                    &((struct SceneActorAnimationTileDrawable *)A(actor)->drawable)->tile;
                u8 *savedSelection;
                selection += variant;
                savedSelection = &A(actor)->selection;
                restart = &A(actor)->restart;
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
                    u32 mask = 4;
                    u32 mode;
                    if (flags & mask) {
                        mode = 6;
                    } else {
                        mode = 4;
                    }
                    Anim_SetKeyframes(
                        &((struct SceneActorAnimationTileDrawable *)A(actor)->drawable)->tile,
                        A(actor)->drawable, (u16)selection, 2, (u16)mode);
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
