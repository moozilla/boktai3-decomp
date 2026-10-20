#include "global.h"
void Anim_LoadKeyframe(void *, void *, u32, u32);
static inline u32 Test(u8 *p, u32 m)
{
    return *(u32 *)(p + 0x1A4) & m;
}
void Registry2388_UpdateTimedEffect(void);
u16 *Registry3F74_FindFrameOffset(void *, s32);
struct FlagAnim {
    u8 pad[8];
    u32 flags;
};
void Registry2388_UpdateIndicators(u8 *p)
{
    u32 bits = *(u16 *)(p + 0x528);
    u8 *resource = p + 0x500;
    u8 *base = p + 0x20;
    u8 *a;
    u16 *pos;
    if (bits & 2)
        Anim_LoadKeyframe(base + 96, resource, 14, 0);
    else
        Anim_LoadKeyframe(base + 96, resource, 13, 0);
    if (bits & 4)
        Anim_LoadKeyframe(base + 192, resource, 16, 0);
    else
        Anim_LoadKeyframe(base + 192, resource, 15, 0);
    if (bits & 8)
        Anim_LoadKeyframe(base + 288, resource, 18, 0);
    else
        Anim_LoadKeyframe(base + 288, resource, 17, 0);
    if (bits & 16)
        Anim_LoadKeyframe(base + 384, resource, 20, 0);
    else
        Anim_LoadKeyframe(base + 384, resource, 19, 0);
    if (bits & 32)
        Anim_LoadKeyframe(base + 480, resource, 22, 0);
    else
        Anim_LoadKeyframe(base + 480, resource, 21, 0);
    if (bits & 64) {
        a = base + 576;
        Anim_LoadKeyframe(a, resource, 23, 0);
        *(u32 *)(a + 8) &= ~1;
    } else
        ((struct FlagAnim *)(base + 576))->flags |= 1;
    if (bits & 128) {
        a = base + 672;
        Anim_LoadKeyframe(a, resource, 24, 0);
        *(u32 *)(a + 8) &= ~1;
    } else
        ((struct FlagAnim *)(base + 672))->flags |= 1;
    if (bits & 256) {
        a = base + 768;
        Anim_LoadKeyframe(a, resource, 26, 0);
        *(u32 *)(a + 8) &= ~1;
    } else
        ((struct FlagAnim *)(base + 768))->flags |= 1;
    if (bits & 1)
        Anim_LoadKeyframe(base, resource, 28, 0);
    else
        Anim_LoadKeyframe(base, resource, 27, 0);
    if (*(s32 *)(p + 0x52C) >= 0 && Test(*(u8 **)(p + 0x558), 2)) {
        ((struct FlagAnim *)(base + 0x360))->flags &= ~1;
        Anim_LoadKeyframe(base + 0x360, resource, (u16)(*(s32 *)(p + 0x52C) + 29), 0);
    } else
        ((struct FlagAnim *)(base + 0x360))->flags |= 1;
    if (*(void (**)(void))(p + 0x55C) == Registry2388_UpdateTimedEffect || !*(u32 *)(p + 0x55C)) {
        ((struct FlagAnim *)(p + 0x440))->flags |= 1;
        ((struct FlagAnim *)(p + 0x4A0))->flags |= 1;
    } else {
        base = p + 0x440;
        pos = Registry3F74_FindFrameOffset(*(void **)(p + 0x558), *(s16 *)(p + 0x530));
        if (pos) {
            *(u16 *)(base + 0x28) = pos[0];
            *(u16 *)(base + 0x2A) = pos[1];
            *(u32 *)(base + 8) &= ~1;
        } else {
            *(u16 *)(base + 0x28) = 0;
            *(u16 *)(base + 0x2A) = 0;
            *(u32 *)(base + 8) |= 1;
        }
        if (*(u16 *)(p + 0x532) <= 8) {
            base = p + 0x4A0;
            pos = Registry3F74_FindFrameOffset(*(void **)(p + 0x558), *(s16 *)(p + 0x532));
            if (pos) {
                *(u16 *)(base + 0x28) = pos[0] + 16;
                *(u16 *)(base + 0x2A) = pos[1];
                *(u32 *)(base + 8) &= ~1;
            } else {
                *(u16 *)(base + 0x28) = 0;
                *(u16 *)(base + 0x2A) = 0;
                *(u32 *)(base + 8) |= 1;
            }
        } else
            ((struct FlagAnim *)(p + 0x4A0))->flags |= 1;
    }
}
