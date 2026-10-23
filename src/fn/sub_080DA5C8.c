#include "global.h"
// confidence=low: clears movement response flags and restores the response speed and sentinel.
#define Mummy_ResetResponseState sub_080DA5C8
struct ResponseState {
    u8 pad[8];
    u16 timer;
    u8 mode;
    u8 padB[13];
    u32 counter;
};
static inline void ClearHalf(u16 *p, u32 mask)
{
    *p = *p & mask;
}
static inline u32 WordFlag(u8 *p, u32 mask, u32 off)
{
    return *(u32 *)(p + off) & mask;
}
void Mummy_ResetResponseState(u8 *p)
{
    u8 *body = p + 0xDC;
    u32 mask = ~0x18;
    u32 zero;
    u32 *flags;
    u16 *motion;
    struct ResponseState *response;
    {
        u32 loaded = body[15];
        u32 value = mask;
        do {
        value &= loaded;
        } while (0);
        zero = 0;
        body[15] = value;
    }
    flags = (u32 *)(p + 0x2D0);
    {
        u32 mask = 0xFEFFFFFF;
        *flags &= mask;
    }
    *(u16 *)(p + 0x2C2) = 64;
    *(u16 *)(p + 0x414) = zero;
    *(u16 *)(p + 0x418) = zero;
    motion = (u16 *)(p + 0x2D4);
    ClearHalf(motion, 0xFFFFFC7F);
    body[15] &= mask;
    response = (void *)(p + 0x420);
    if (WordFlag(p, 0x200000, 0x2CC)) {
        {
            u32 mask = 0xFEFFFFFF;
            *flags &= mask;
        }
        ClearHalf(motion, 0xFFFFFDFF);
        response->mode = 0;
        response->counter = zero;
        response->timer = zero;
    }
    ClearHalf(motion, 0xFFFFFBFF);
    {
        u32 offset = 0x5DA;
        u8 *address = p + offset;
        *address = 255;
    }
}
