#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Complete transition; flag setters and reset offsets still allocate differently.
#include "ivy_phase.h"

void Ivy_UpdateActiveState(void);

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
static inline void Ivy_ClearHalf(u8 *p, u32 offset, u32 bits)
{
    u16 *field = (void *)(p + offset);
    s32 mask = ~bits;
    *field &= mask;
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
    *field = value & 0xFFF7;
}

static inline void Ivy_EnterActive(u8 *p, void (*callback)(void), u32 kind)
{
    u8 *prepared = &p[0x2B2];
    u32 zero = 0;
    u32 one = 1;
    *prepared = one;
    p[0x2B0] = zero;
    p[0x2B5] = kind;
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

inline static void Ivy_ClearHalf_UpdateAnimationTransition(u8 *p, u8 offset, u32 bits)
{
    u16 *field = (void *)(p + offset);
    s32 mask = ~bits;
    *field &= mask;
}

void SceneActorControl_UpdateAnimationTransition(u8 *p, s32 frame)
{
    struct IvyTransitionAnimation *animation = (void *)((*((u8 **)(p + 4))) + 0x48);
    if (SceneActorControl_TakeFlag(&p[0x2B2])) {
        SceneActorAnimation_SelectDirectionalFlags(p, 3);
        p[0x2B3] = 0;
        Ivy_SetBlend(p);
        Ivy_SetHalf(p, 0x2C0, 0x800);
        Ivy_SetHalf(p, 0x2D4, 0x20);
    }
    Ivy_SetHalf(p, 0x2D6, 0x10);
    if ((((frame <= 17) && (animation->count == 0)) &&
         (animation->frame == (animation->mode != 0))) &&
        (animation->mode != 1))
        animation->frame = 0;
    if (p[0x2B3]) {
        p[0x2B0] = 0;
        Ivy_ClearHalf_UpdateAnimationTransition(p, 0x2D4, 0x20);
        Ivy_ClearWord(p, 0x2CC, 0x80);
        Ivy_EnterActive(p, Ivy_UpdateActiveState, 0x22);
        {
            u32 value = p[0xE9];
            p[0x59] = value;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorControl_UpdateAnimationTransition);
#endif
