#include "global.h"
struct FlashTask {
    u8 pad0[0x18];
    u32 flags;
    u8 pad1C[0x28 - 0x1C];
    u16 frame;
    u8 pad2A[0x6A - 0x2A];
    u16 baseFrame;
    u16 kind;
    u8 pad6E[0x76 - 0x6E];
    u16 timer;
};
void Task_Kill(void *);
static inline u32 Flip(u16 kind)
{
    u32 flip = 0;
    if (kind > 2) {
        flip = 8;
        if (kind > 4) {
            flip = 4;
            if (kind <= 5)
                flip = 0xC;
        }
    }
    return flip;
}
// confidence=low: three frames from baseFrame, then three of a frame and flip picked by kind;
// kills itself after six frames.
void sub_082373D4(struct FlashTask *p)
{
    if (p->timer <= 2)
        p->frame = (p->baseFrame >> 1) + 4;
    else {
        s32 offset;
        if (p->kind & 1)
            offset = 1;
        else
            offset = (p->kind >> 1) & 1 ? 2 : 0;
        p->flags |= Flip(p->kind);
        p->frame = offset + 6;
    }
    if (++p->timer > 5)
        Task_Kill(p);
}
