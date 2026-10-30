#include "global.h"
#ifdef NONMATCHING
// fuzzy: 64.8%
// fix.py best version
// fix.py best version (reprinted C)
// confidence=low: selects turn animation and rotates toward the route facing.
// MATCHING_CONCESSION: register-pinning
// Shared animation tails, branch layout, and flag-pointer allocation still differ.
#include "scene_actor_chase.h"
// confidence=low: chooses turn animations and rotates toward the route's facing.
u32 Route_GetCurrentPointFacing(void *);
struct TurnSequence {
    u8 pad[8];
    u16 frame;
    u8 padA[4];
    u16 timer;
};
static inline void SelectTurn(struct SceneActorAnimationView *actor, u32 selection, u32 variant,
                              u32 flags)
{
    u32 force;
    register u8 *restart asm("r4");
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
        {
            u8 *selectionAddress = &actor->selection;
            u32 previous = *selectionAddress;
            savedSelection = selectionAddress;
            restart = &actor->restart;
            if (!force && previous == selection && !*restart) {
                if (flags == 1 && (tile->flags & 4))
                    return;
                if (flags == 0 && !(tile->flags & 4))
                    return;
            }
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
static inline u32 TurnFlags(u16 *field, u32 mask)
{
    return *field & mask;
}
static inline void SelectTurn_1(struct SceneActorAnimationView *actor, u32 selection, u32 flags)
{
    u32 force;
    register u8 *restart asm("r4");
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection, (u8)0,
                         (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += 0;
        {
            u8 *selectionAddress = &actor->selection;
            u32 previous = *selectionAddress;
            savedSelection = selectionAddress;
            restart = &actor->restart;
            if (((!force) && (previous == selection)) && (!(*restart))) {
                if ((flags == 1) && (tile->flags & 4))
                    return;
                if ((flags == 0) && (!(tile->flags & 4)))
                    return;
            }
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
    {
        u8 k1 = 0;
        *restart = k1;
    }
}

void Mummy_TurnTowardRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *direction = ((u8 *)actor) + 0x50;
    struct TurnSequence *sequence = (void *)(((u8 *)actor->drawable) + 0x48);
    u8 *group = (u8 *)(&actor->group.common);
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(actor);
    motionFlags = &actor->flags2D4;
    if (prepared) {
        s32 quadrant;
        s32 facing;
        direction[9] = ((((direction[9] + 0x60) >> 6) & 3) * 64) - 64;
        quadrant = ((direction[9] + 0x60) >> 6) & 3;
        if ((actor->behavior2AA != 6) && (!TurnFlags(motionFlags, 0x480))) {
            facing =
                ((((u16)Route_GetCurrentPointFacing(*((void **)(direction + 0x28)))) + 0x70) >> 5) &
                7;
            ((struct SceneActorAnimationView *)actor)->nextDirection = facing;
        } else
            facing = ((struct SceneActorAnimationView *)actor)->nextDirection;
        if (quadrant == (facing >> 1)) {
            actor->field2B0 = 1;
            if (TurnFlags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                Chase_SetState(actor, 3, 1, 0);
                Chase_SetCtl(actor, *((void **)(group + 0x21C)), 0x1B);
            } else
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            return;
        }
        ((struct SceneActorAnimationView *)actor)->nextDirection = facing;
        switch (quadrant) {
            case 0:
                if (((u32)(facing - 2)) <= 3) {
                    SelectTurn((void *)actor, 2, 1, 0);
                    goto forward;
                } else {
                    SelectTurn_1((void *)actor, 2, 0);
                    goto backward;
                }
                break;

            case 1:
                if (((u32)(facing - 4)) <= 3) {
                    SelectTurn((void *)actor, 2, 2, 0);
                    goto forward;
                } else {
                    SelectTurn((void *)actor, 2, 1, 4);
                    goto backward;
                }
                break;

            case 2:
                if (((u32)(facing - 2)) > 3) {
                    SelectTurn((void *)actor, 2, 1, 5);
                    goto forward;
                } else {
                    SelectTurn((void *)actor, 2, 2, 4);
                    goto backward;
                }
                break;

            case 3:
                if (((u32)facing) <= 3) {
                    SelectTurn((void *)actor, 2, 0, 4);
                    goto forward;
                } else {
                    SelectTurn((void *)actor, 2, 1, 1);
                    goto backward;
                }
                break;
        }

        goto entryDone;
    forward: {
        s16 mask = 0x10000;
        mask |= actor->flags2CC;
        actor->flags2CC = mask;
    }

        goto entryDone;
    backward:
        actor->flags2CC &= ~0x10000;

    entryDone:
        actor->field2B3 = 0;
    }
    if (!sequence->timer) {
        s32 desired = ((((struct SceneActorAnimationView *)actor)->nextDirection + 5) & 7) << 5;
        s32 difference = direction[9] - desired;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x15)
            direction[9] = desired;
        else if (Chase_Bits32(&actor->flags2CC, 0x10000))
            direction[9] += 0x15;
        else
            direction[9] -= 0x15;
    }
    {
        u32 k1 = 0xA02F;
        if ((*motionFlags) & k1)
            actor->field2C2 = 0x40;
        else if (!actor->field2C2)
            actor->field2C2 = 0x40;
    }
    if (!actor->drawableKind)
        Anim_SetDurationScale(((u8 *)actor->drawable) + 0x48, actor->field2C2);
    else
        Anim_SetFrameDurationScale(((u8 *)actor->drawable) + 0x20, actor->field2C2);
    if (actor->field2B3) {
        s32 facing = ((direction[9] + 0x70) >> 5) & 7;
        s32 difference = ((struct SceneActorAnimationView *)actor)->nextDirection - facing;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0) {
            actor->field2B0 = 1;
            actor->body.direction = ((((actor->body.direction + 0x60) >> 6) & 3) * 64) - 64;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            if (TurnFlags(motionFlags, 0x400)) {
                Chase_SetState(actor, 3, 1, 0);
                Chase_SetCtl(actor, *((void **)(group + 0x21C)), 0x1B);
            }
        } else
            Chase_SetCtl(actor, actor->entryTarget224, 2);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Mummy_TurnTowardRoute);
#endif
