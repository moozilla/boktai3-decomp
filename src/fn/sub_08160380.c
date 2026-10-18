#include "global.h"
#define FrameDisplay_GetCompactFrameIndex sub_08160380
struct FrameDisplayEntry { u16 frame, unknown2; };
struct FrameDisplayAnimation {
    u8 unknown0[0x14];
    u16 frameIndex;
    u8 unknown16[0x3e];
    struct FrameDisplayEntry *frames;
};
struct FrameDisplay {
    u8 unknown0[0x74];
    struct FrameDisplayAnimation *animation;
};
s32 sub_08160380(struct FrameDisplay *p)
{
    s32 frame = p->animation->frames[p->animation->frameIndex].frame;
    if ((u32)(frame - 0xeb) <= 0x3a) {
        if (frame == 0xff) frame = 0xfe;
        if (frame == 0x100) frame = 0xff;
        if (frame == 0x101) frame = 0x100;
        if (frame == 0x10b) frame = 0x10a;
        if (frame == 0x10c) frame = 0x10b;
        if (frame == 0x10d) frame = 0x10c;
        if (frame == 0x117) frame = 0x116;
        if (frame == 0x118) frame = 0x117;
        if (frame == 0x119) frame = 0x118;
        if (frame == 0x123) frame = 0x122;
        if (frame == 0x124) frame = 0x123;
        if (frame == 0x125) frame = 0x124;
        if (frame > 0x101) frame--;
        if (frame > 0x10d) frame--;
        if (frame > 0x119) frame--;
        return frame - 0xaa;
    }
    if ((u32)(frame - 0xaa) <= 0x40) return frame - 0xaa;
    return -1;
}
