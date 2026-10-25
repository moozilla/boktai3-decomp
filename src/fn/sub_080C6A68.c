// confidence=low: existing Golem state interpretation retained.
// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
#ifdef NONMATCHING
// fuzzy: 91.1%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: stack-slot placement, packed-store scheduling and animation pointer copies.
/* Decelerate the charge, return to the current cell center, then restore the saved route. */
// confidence=low: installed when charge movement completes; the three phases slow, center, and settle it.
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_terrain.h"
s32 sub_080C47DC(void *);

extern const u16 gUnk_08606130[];
void sub_080C47B8(struct ContactPosition *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

struct RecoverySample {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct GolemChargeMotion {
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

void Golem_UpdateChargeRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    struct GolemChargeMotion *motion = (struct GolemChargeMotion *)(((u8 *)actor) + 0x50);
    s16 *speed;
    u16 *flags2D4;
    u16 *initialFlags;
    register u32 animationZero asm("r6");
    if (TakeFlag(&actor->field2B2)) {
        actor->field2B0 = 0;
        actor->field2B3 = 0;
        body[0x68D] = 0;
        {
            u32 angle = (body[0x690] + 0x60) >> 6;
            u32 quadrant = 3;
            quadrant &= angle;
            body[0x68C] = quadrant;
        }
        *((struct ContactPosition *)(body + 0x69C)) = motion->position;
        sub_080C47B8((struct ContactPosition *)(body + 0x69C));
    }
    {
        u32 mask = 0x100;
        u16 *flags = &actor->flags2D4;
        register u32 value asm("r1") = *flags;
        initialFlags = flags;
        animationZero = 0;
        *flags = mask | value;
    }
    {
        u8 directionVariant;
        u8 mirror;
        register u8 *mirrorPointer asm("r2");
        register u8 *variantPointer asm("r5");
        u32 flags;
        u32 variant;
        u32 selection;
        u8 *variantReload;
        u32 force;
        u8 *restart;
        variantPointer = &directionVariant;
        {
            u8 *mirrorAddress = &mirror;
            s32 quadrant = ((((s32)actor->direction) + 0x60) >> 6) & 3;
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
            if (((!force) && ((*savedSelection) == selection)) && (!(*restart))) {
                if ((flags == 1) && (tile->flags & 4))
                    goto done;
                if ((flags == 0) && (!(tile->flags & 4)))
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
        *((s16 *)(body + 0x682)) -= 0x28;
        speed = (s16 *)(body + 0x682);
        if ((*speed) < 0) {
            u8 *phase = body + 0x68D;
            u32 zero = 0;
            *phase = 1;
            *speed = zero;
            return;
        }
        {
            struct RecoverySample step;
            struct RecoverySample candidate;
            struct RecoverySample *out;
            struct ContactPosition *p;
            s32 x;
            s32 z;
            s32 cell;
            step.x = ((*speed) * gUnk_086149C4[(body[0x691] + 0x40) & 0xFF]) >> 16;
            do {
                step.z = ((*speed) * gUnk_086149C4[body[0x691]]) >> 16;
            } while (0);
            candidate = *((struct RecoverySample *)(&actor->position));
            out = (struct RecoverySample *)(&candidate);
            *((u32 *)out) =
                ((*((u32 *)out)) & 0xFFFF0000) | ((u16)(((u16)candidate.x) + ((u16)step.x)));
            ((u32 *)out)[1] =
                (((u32 *)out)[1] & 0xFFFF0000) | ((u16)(((u16)candidate.z) + ((u16)step.z)));
            p = (struct ContactPosition *)(&candidate);
            x = p->x >> 8;
            z = p->z >> 8;
            if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) ||
                (((u32)z) >= gUnk_030052FC))
                cell = 0;
            else
                cell = gUnk_030052F4->rows[z] + x;
            if (actor->terrainCell != cell) {
                *((u16 *)(body + 0x682)) = 0;
                body[0x68D] = 1;
            } else {
                actor->motion.x += step.x;
                actor->motion.z += step.z;
            }
        }
    } else if (body[0x68D] == 1) {
        struct ContactPosition delta;
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(actor->terrainCell, &center);
        Delta(&delta, &actor->position, &center);
        switch (body[0x691] >> 6) {
            case 0:

            case 2:
                delta.z = 0;
                if (delta.x > 8)
                    motion->x = 8;
                else if (delta.x < (-8))
                    motion->x = -8;
                else
                    motion->x = delta.x;
                break;

            case 1:

            case 3:
                delta.x = 0;
                if (delta.z > 8)
                    motion->z = 8;
                else if (delta.z < (-8))
                    motion->z = -8;
                else
                    motion->z = delta.z;
                break;
        }

        if ((delta.x + delta.z) == 0) {
            body[0x68D] = 2;
            body[0x68F] = 0;
            return;
        }
    } else {
        u8 *counter = body + 0x68F;
        u32 value = (*counter) + 1;
        u32 zero = 0;
        *counter = value;
        if (((u8)value) > 15) {
            *((struct ContactPosition *)(body + 0x69C)) =
                *((struct ContactPosition *)(body + 0x694));
            sub_080C47DC(body);
            body[0x690] = body[0x68E];
            actor->direction = body[0x68E];
            actor->field2B0 = 1;
            *counter = zero;
            *((u16 *)(body + 0x682)) = zero;
            return;
        }
    }

    actor->field2C2 = 0x40 >> ((*((u16 *)(body + 0x684))) >> 7);
    asm("" : "+r"(actor));
    {
        u32 mask = 0xA02F;
        u16 *scale;
        if ((*flags2D4) & mask) {
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
            Anim_SetDurationScale(((u8 *)actor->drawable) + 0x48, *scale);
        else
            Anim_SetFrameDurationScale(((u8 *)actor->drawable) + 0x20, *scale);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Golem_UpdateChargeRecovery);
#endif
