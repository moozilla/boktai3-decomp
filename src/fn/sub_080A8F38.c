#include "global.h"
// MATCHING_CONCESSION: register-pinning

// confidence=low: refreshes segment animations from their direction, keeping frame and timer.
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
static inline u8 TestMask2(struct SegmentAnim *p, u32 mask)
{
    register u32 m asm("r1") = mask;
    if (*(volatile u8 *)&p->status & m)
        return 1;
    return 0;
}
static inline u8 TestMask(struct SegmentAnim *p)
{
    u32 f = p->status;
    if (f & 1)
        return 1;
    return 0;
}
static inline void *ScriptAt(u8 *body, s32 offset)
{
    u32 address = 0x4d4;
    address += (u32)body;
    address += offset;
    return *(void **)address;
}
static inline void Step(struct SegmentAnim *input, struct Segment *sprite)
{
    struct Segment *seg = sprite;
    struct Segment *o = seg;
    struct SegmentAnim *a = input;
    struct SegmentAnim *b = input;
    const volatile u16 *entry;
    entry = a->data + a->frame;
    {
        u32 frame = *entry >> 6;
        seg->frame = frame;
    }
    {
        u32 input = a->flags;
        u32 left = 1;
        left &= input;
        if (left != (((*entry & 0x30) >> 4) & 1))
            seg->flags |= 4;
        else
            seg->flags &= ~4;
    }
    {
        register u32 input asm("r0") = b->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    {
        u32 elapsed = b->timer + 1;
        u32 mask;
        register u32 temp asm("r2");
        register u32 savedDuration asm("r2");
        u32 duration, flags;
        b->timer = elapsed;
        temp = 0xffff;
        asm("" : "+r"(temp));
        mask = temp;
        duration = b->rate;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            b->timer = 0;
            flags = b->flags;
            if (flags & 4) {
                u32 index = b->frame;
                if (index == 0) {
                    u32 f8 = 8;
                    f8 &= flags;
                    if (f8)
                        b->timer = savedDuration;
                    else
                        b->frame = b->count - 1;
                } else
                    b->frame = index - 1;
            } else {
                u32 index = b->frame + 1;
                u32 frameCount;
                b->frame = index;
                frameCount = b->count;
                if ((index & mask) >= frameCount) {
                    u32 endFlag = 8;
                    endFlag &= flags;
                    if ((u8)endFlag) {
                        b->timer = duration;
                        b->frame = frameCount - 1;
                    } else
                        b->frame = 0;
                }
            }
            entry = b->data + b->frame;
            b->factor = *entry & 15;
            {
                u32 r = (b->factor * b->scale) >> 6;
                b->rate = r;
                if (!(u8)r) {
                    u32 one = 1;
                    b->rate = one;
                }
            }
        }
    }
}
void TentacleBody_RefreshDirectionalAnimations(u8 *input, s32 inputKind)
{
    u8 *body = input;
    struct {
        s32 row, flags, kind;
        u8 *profile, *actor;
        s32 i;
    } frame;
    s32 offset;
    s32 four;
    u16 frameSave, timerSave;
    struct SegmentAnim *a;
    struct Segment *s;
    frame.kind = inputKind;
    frame.actor = body;
    frame.profile =
        SceneActorProfiles_GetEntryAddress(FIELD(body, 0x308, u32), FIELD(body, 0x2c4, u32), 0);
    frame.i = 0;
    if (frame.i >= FIELD(body, 0x8da, s16))
        return;
    offset = 0;
    a = (void *)(body + 0x4c4);
    s = (void *)(body + 0x3d4);
    do {
        if (!TestMask(a)) {
            if (TestMask2(a, 4)) {
                TentacleBody_GetDirectionFrame((FIELD(a, 0x14, u8) + 128) & 255, &frame.row,
                                               &frame.flags);
                Anim_SetSequence(a, ScriptAt(body, offset), 1, frame.row, frame.flags);
            } else {
                TentacleBody_GetDirectionFrame(FIELD(a, 0x14, u8), &frame.row, &frame.flags);
                four = 4;
                frameSave = a->frame;
                timerSave = a->timer;
                Anim_SetSequence(a, ScriptAt(body, offset),
                                 *(u8 *)(({
                                             register s32 k asm("r2") = frame.kind;
                                             register s32 x asm("r0") = k * four;
                                             x;
                                         }) +
                                         (u32)frame.profile),
                                 frame.row, frame.flags);
                {
                    register u8 *act asm("r1") = frame.actor;
                    register u16 *ad asm("r0");
                    ad = (u16 *)(act + 0x2c2);
                    Anim_SetDurationScale(a, *ad);
                }
                a->frame = frameSave;
                a->timer = timerSave;
            }
            Step(a, s);
        }
        {
            register u32 step asm("r1") = 0x128;
            offset += step;
            a = (void *)((u8 *)a + step);
            s = (void *)((u8 *)s + step);
        }
    } while (({
                 register s32 t asm("r2") = frame.i + 1;
                 frame.i = t;
                 t;
             }) < FIELD(body, 0x8da, s16));
}
