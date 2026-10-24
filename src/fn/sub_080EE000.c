#include "global.h"
#include "scene_actor_control_phase.h"

struct IvyTransitionActor {
    u8 unknown0[0x59];
    u8 direction;
    u8 unknown5A[0x8F];
    u8 savedDirection;
    u8 unknownEA[0x1A];
    u32 motion;
    u8 unknown108[0xC];
    u32 flags;
    u8 unknown118[0x22];
    u16 blendFlags;
    u8 unknown13C[0xE0];
    void (*callback)(void);
    u8 unknown220[0x88];
    u8 phase;
    u8 subphase;
    u8 phaseFlags;
    u8 unknown2AB[0x1];
    u32 phaseFrames;
    u8 active;
    u8 restart;
    u8 prepared;
    u8 complete;
    u8 unknown2B4[0x1];
    u8 kind;
    u8 unknown2B6[0x12];
    u16 timer;
    u8 unknown2CA[0x2];
    u32 controlFlags;
    u8 unknown2D0[0x4];
    u16 animationFlags;
};

void Ivy_UpdateActiveState(void);

static inline void Ivy_ResetPhase(u8 *p)
{
    u8 *state = &((struct IvyTransitionActor *)p)->phase;
    u32 zero = 0;
    *state = zero;
    ((struct IvyTransitionActor *)p)->subphase = zero;
    ((struct IvyTransitionActor *)p)->phaseFlags = zero;
    *(u32 *)(p + 0x2AC) = zero;
    ((struct IvyTransitionActor *)p)->restart = 1;
}

struct IvyTransitionAnimation {
    u8 unknown0[7];
    u8 mode;
    u16 count;
    u8 unknownA[4];
    u16 frame;
};

static inline void Ivy_SetHalf(u8 *p, u32 offset, u32 mask)
{
    u32 value = mask;
    u16 *field = (void *)(p + offset);
    value |= *field;
    *field = value;
}
static inline void Ivy_ClearWord(u8 *p, u32 offset, u32 bits)
{
    u32 *field = (void *)(p + offset);
    s32 mask = ~bits;
    *field &= mask;
}
static inline void Ivy_SetBlend(u8 *p)
{
    u16 *field = (void *)(p + 0x13A);
    u32 old = *field;
    u32 value = 4;
    value |= old;
    value &= 0xFFF7;
    *field = value;
}

static inline void Ivy_EnterActive(u8 *p, void (*callback)(void), u32 kind)
{
    u8 *prepared = &((struct IvyTransitionActor *)p)->prepared;
    u32 zero = 0;
    u32 one = 1;
    *prepared = one;
    ((struct IvyTransitionActor *)p)->active = zero;
    ((struct IvyTransitionActor *)p)->kind = kind;
    *(void (**)(void))(p + 0x21C) = callback;
    *(u16 *)(p + 0x2C8) = zero;
    {
        u32 *flags = (u32 *)(p + 0x114);
        u32 mask = ~1;
        *flags &= mask;
    }
    *(u32 *)(p + 0x104) = zero;
    Ivy_ResetPhase(p);
}

static inline void Ivy_ClearHalf(u8 *p, u32 offset, u32 bits)
{
    struct IvyHalf {
        u16 value;
    };
    struct IvyHalf *field = (void *)(p + offset);
    s32 mask = ~bits;
    field->value &= mask;
}

void SceneActorControl_UpdateAnimationTransition(u8 *p, s32 frame)
{
    struct IvyTransitionAnimation *animation = (void *)((*((u8 **)(p + 4))) + 0x48);
    if (SceneActorControl_TakeFlag(&((struct IvyTransitionActor *)p)->prepared)) {
        SceneActorAnimation_SelectDirectionalFlags((struct SceneActorAnimationView *)p, 3);
        ((struct IvyTransitionActor *)p)->complete = 0;
        Ivy_SetBlend(p);
        Ivy_SetHalf(p, 0x2C0, 0x800);
        Ivy_SetHalf(p, 0x2D4, 0x20);
    }
    Ivy_SetHalf(p, 0x2D6, 0x10);
    if ((((frame <= 17) && (animation->count == 0)) &&
         (animation->frame == (animation->mode != 0))) &&
        (animation->mode != 1))
        animation->frame = 0;
    if (((struct IvyTransitionActor *)p)->complete) {
        ((struct IvyTransitionActor *)p)->active = 0;
        Ivy_ClearHalf(p, 0x2D4, 0x20);
        Ivy_ClearWord(p, 0x2CC, 0x80);
        Ivy_EnterActive(p, Ivy_UpdateActiveState, 0x22);
        {
            u32 value = ((struct IvyTransitionActor *)p)->savedDirection;
            ((struct IvyTransitionActor *)p)->direction = value;
        }
    }
}
