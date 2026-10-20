#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define TentacleBody_RefreshDirectionalAnimations sub_080A8F38

#ifdef NONMATCHING
// fuzzy: 85.0%
// Remaining differences: stack layout, register allocation and instruction ordering.
struct SegmentAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
    void *script;
    u8 direction, status;
};
struct Segment {
    u32 flags;
    u8 pad4[12];
    u16 frame;
    u8 pad12[0xde];
    struct SegmentAnim anim;
    u8 pad108[0x20];
};
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
void *SceneActorProfiles_GetEntryAddress(u32, u32, s32);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_SetSequence(void *, void *, u32, u8, u8);
void Anim_RestartSequence(void *, void *, u32, u8, u8);
void Anim_SetDurationScale(void *, u32);
static inline u8 TestMask(struct SegmentAnim *p, u32 mask)
{
    u32 f = p->status;
    asm("" : "+r"(f));
    if (f & mask)
        return 1;
    return 0;
}
static inline void *ScriptAt(u8 *body, s32 offset)
{
    register u32 address asm("r0") = 0x4d4;
    address += (u32)body;
    asm("" : "+r"(address));
    address += offset;
    return *(void **)address;
}
static inline void Step(struct SegmentAnim *input, struct Segment *sprite)
{
    struct Segment *o = sprite;
    struct SegmentAnim *a = input;
    const volatile u16 *entry;
    asm("" : "+r"(o), "+r"(a));
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        u32 left = 1;
        u32 mask = 1;
        left &= input;
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    {
        u32 elapsed = a->timer + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r2");
        u32 duration, flags;
        a->timer = elapsed;
        temp = 0xffff;
        asm("" : "+r"(temp));
        mask = temp;
        duration = a->rate;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            a->timer = 0;
            flags = a->flags;
            if (flags & 4) {
                u32 index = a->frame;
                if (index == 0) {
                    if (flags & 8)
                        a->timer = savedDuration;
                    else
                        a->frame = a->count - 1;
                } else
                    a->frame = index - 1;
            } else {
                u32 index = a->frame + 1;
                u32 frameCount;
                a->frame = index;
                frameCount = a->count;
                if ((index & mask) >= frameCount) {
                    u32 endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    if ((u8)endFlag) {
                        a->timer = duration;
                        a->frame = frameCount - 1;
                    } else
                        a->frame = 0;
                }
            }
            entry = a->data + a->frame;
            a->factor = *entry & 15;
            a->rate = (a->factor * a->scale) >> 6;
            if (!a->rate)
                a->rate = 1;
        }
    }
}
void TentacleBody_RefreshDirectionalAnimations(u8 *input, s32 kind)
{
    register u8 *body asm("r9") = input;
    u8 *actor = body;
    u8 *profile =
        SceneActorProfiles_GetEntryAddress(FIELD(body, 0x308, u32), FIELD(body, 0x2c4, u32), 0);
    s32 i;
    s32 row, flags;
    register s32 offset asm("r10");
    register struct SegmentAnim *a asm("r6");
    register struct Segment *s asm("r8");
    i = 0;
    if (i >= FIELD(body, 0x8da, s16))
        return;
    offset = 0;
    a = (void *)(body + 0x4c4);
    s = (void *)(body + 0x3d4);
    do {
        if (!TestMask(a, 1)) {
            if (TestMask(a, 4)) {
                TentacleBody_GetDirectionFrame((FIELD(a, 0x14, u8) + 128) & 255, &row, &flags);
                Anim_SetSequence(a, ScriptAt(body, offset), 1, row, flags);
            } else {
                u16 frame, timer;
                TentacleBody_GetDirectionFrame(FIELD(a, 0x14, u8), &row, &flags);
                frame = a->frame;
                timer = a->timer;
                Anim_SetSequence(a, ScriptAt(body, offset), profile[kind * 4], row, flags);
                Anim_SetDurationScale(a, FIELD(actor, 0x2c2, u16));
                a->frame = frame;
                a->timer = timer;
            }
            Step(a, s);
        }
        offset += 0x128;
        a = (void *)((u8 *)a + 0x128);
        s = (void *)((u8 *)s + 0x128);
    } while (++i < FIELD(body, 0x8da, s16));
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080A8F38);
#endif
