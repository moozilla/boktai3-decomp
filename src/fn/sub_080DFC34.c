#include "global.h"
#include "scene_actor_animation.h"

struct DogTurnMotion {
    u8 unknown0[9];
    u8 direction;
    u8 unknownA[0x1E];
    void *route;
};
struct DogTurnBody {
    u8 unknown0[0x21C];
    void *resumeControl;
};
struct DogTurnState {
    void *object;
    void *drawable;
    u8 drawableKind;
    u8 unknown9[0x47];
    struct DogTurnMotion motion;
    u8 unknown7C[0x88];
    u32 motionCounter;
    u8 unknown108[0xC];
    u32 motionFlags;
    u8 unknown118[0x104];
    void *control;
    u8 unknown220[4];
    void *turnControl;
    u8 unknown228[0x80];
    u8 state, phase, behavior, unknown2AB;
    u32 counter;
    u8 ready, phaseReady, prepared, restart;
    u8 unknown2B4, controlKind, unknown2B6, selection;
    u8 unknown2B8[0xA];
    u16 duration;
    u32 profileMode;
    u16 controlTimer, unknown2CA;
    u32 flags;
    u32 unknown2D0;
    u16 flags2D4;
    u8 unknown2D6[0xF7];
    u8 targetDirection;
    u8 unknown3CE[6];
    struct DogTurnBody body;
};

u32 Route_GetCurrentPointFacing(void *);
s32 Div(s32, s32);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

static inline s32 DogTurn_Cardinal(u8 direction)
{
    return ((direction + 0x60) >> 6) & 3;
}
struct DogTurnSequence {
    u8 unknown0[0xE];
    u16 elapsed;
};
static inline u16 DogTurn_Elapsed(void *sequence)
{
    return ((struct DogTurnSequence *)sequence)->elapsed;
}
static inline u32 DogTurn_ActorFlags(struct DogTurnState *a, u32 mask)
{
    return a->flags & mask;
}
static inline void DogTurn_ApplyDuration(struct DogTurnState *a, u16 *motionFlags)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (*motionFlags & mask) {
        u16 *p = &a->duration;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &a->duration;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!a->drawableKind)
        Anim_SetDurationScale(((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence,
                              *scale);
    else
        Anim_SetFrameDurationScale(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile,
                                   *scale);
}
static inline u8 DogTurn_TakePrepared(struct DogTurnState *a)
{
    if (a->prepared) {
        a->prepared = 0;
        return 1;
    }
    return 0;
}
static inline s32 DogTurn_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}
static inline u32 DogTurn_Flags(u16 *field, u32 mask)
{
    return *field & mask;
}
static inline void DogTurn_SetBits(u32 bits, u32 *field)
{
    *field |= bits;
}
static inline void DogTurn_ClearBits(u32 *field, u32 bits)
{
    *field &= ~bits;
}
static inline void DogTurn_SetState(struct DogTurnState *a, u32 state, u32 phase, u32 behavior)
{
    a->state = state;
    a->phase = phase;
    a->behavior = behavior;
    a->counter = 0;
    a->phaseReady = 1;
}
static inline void DogTurn_SetControl(struct DogTurnState *a, void *control, u32 kind)
{
    a->prepared = 1;
    a->ready = 0;
    a->controlKind = kind;
    a->control = control;
    a->controlTimer = 0;
    {
        u32 *field = &a->motionFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    a->motionCounter = 0;
}
static inline void DogTurn_SelectAnimation(struct DogTurnState *a, u32 selection, u32 variant,
                                           u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!a->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        a->selection = selection;
        drawable = a->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)a->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        {
            u8 *selectionAddress = &a->selection;
            u32 previous = *selectionAddress;
            savedSelection = selectionAddress;
            if (!force && previous == selection && !a->restart) {
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
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile,
                              a->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    a->restart = 0;
}

void sub_080DFC34(struct DogTurnState *a)
{
    struct DogTurnBody *body = &a->body;
    struct DogTurnMotion *motion = &a->motion;
    u16 *motionFlags;
    u8 prepared = DogTurn_TakePrepared(a);
    s32 direction, target, delta;
    u32 oldDuration;

    motionFlags = &a->flags2D4;
    if (prepared) {
        a->motion.direction = (DogTurn_Cardinal(a->motion.direction) << 6) - 0x40;
        direction = DogTurn_Cardinal(a->motion.direction);
        if (a->behavior != 6 && !(DogTurn_Flags(motionFlags, 0x480))) {
            target = (((u16)Route_GetCurrentPointFacing(motion->route) + 0x70) >> 5) & 7;
            a->targetDirection = target;
        } else {
            target = a->targetDirection;
        }
        if (direction == DogTurn_Half(target)) {
            a->ready = 1;
            if (DogTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
                DogTurn_SetState(a, 3, 1, 0);
                DogTurn_SetControl(a, body->resumeControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            }
            return;
        }
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    DogTurn_SelectAnimation(a, 13, 1, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 0, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    DogTurn_SelectAnimation(a, 13, 2, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 3, 0);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    DogTurn_SelectAnimation(a, 13, 3, 1);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 2, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    DogTurn_SelectAnimation(a, 13, 0, 0);
                    DogTurn_SetBits(0x10000, &a->flags);
                } else {
                    DogTurn_SelectAnimation(a, 13, 1, 1);
                    DogTurn_ClearBits(&a->flags, 0x10000);
                }
                break;
        }
        a->restart = 0;
    }
    if (DogTurn_Elapsed(((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence) ==
        1) {
        if (DogTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    oldDuration = a->duration;
    if (DogTurn_Flags(motionFlags, 0x80))
        a->duration = Div(0x40, 3);
    DogTurn_ApplyDuration(a, motionFlags);
    a->duration = oldDuration;
    if (a->restart) {
        s32 current = ((a->motion.direction + 0x70) >> 5) & 7;
        delta = a->targetDirection - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            a->ready = 1;
            a->motion.direction = (DogTurn_Cardinal(a->motion.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (DogTurn_Flags(motionFlags, 0x400)) {
                DogTurn_SetState(a, 3, 1, 0);
                DogTurn_SetControl(a, body->resumeControl, 0x1B);
            }
        } else {
            DogTurn_SetControl(a, a->turnControl, 2);
        }
    }
}
