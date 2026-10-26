#include "global.h"
#include "render.h"

struct ContactPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
s32 Anim_InitPositionedWidget(void *, void *, u32, u32, u32, u32, u32, void *);

void Anim_LinkToList(void *, u32);

s32 Widget_InitLinked(u8 *w, void *resource, u16 glyph, u32 flags, u8 a, u8 b, u8 c,
                      const struct ContactPosition *position)
{
    struct ContactPosition pos;
    u32 zero;

    if (position == 0) {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
    } else
        pos = *position;
    zero = *(u8 *)(w + 4);
    if (zero == 0) {
        *(u16 *)(w + 0) = zero;
        *(u16 *)(w + 6) = zero;
        *(u16 *)(w + 0x12) = zero;
        *(u16 *)(w + 0x14) = zero;
        *(u16 *)(w + 0x16) = zero;
        *(u16 *)(w + 0x18) = zero;
        *(u8 *)(w + 0x1d) = 0;
        *(u16 *)(w + 0x28) = zero;
        *(u16 *)(w + 0x2a) = zero;
        *(u16 *)(w + 0x2c) = zero;
        *(u16 *)(w + 0x2e) = zero;
        *(u16 *)(w + 0x30) = zero;
        *(u16 *)(w + 0x32) = zero;
        *(u8 *)(w + 0x34) = 0;
        *(u8 *)(w + 0x35) = 0;
        *(u16 *)(w + 0x36) = zero;
        *(u32 *)(w + 0x3c) = zero;
        *(u32 *)(w + 0x54) = zero;
        *(u32 *)(w + 0x48) = zero;
        *(u32 *)(w + 0x4c) = zero;
        *(u32 *)(w + 0x50) = zero;
        Anim_InitPositionedWidget(w, resource, glyph, flags, a, b, c, &pos);
        gDefaultAnimFlags++;
        if (gDefaultAnimFlags == 0xFFFF)
            gDefaultAnimFlags = zero;
        *(u32 *)(w + 0x40) = -1;
        *(u32 *)(w + 0x44) = -1;
        *(u32 *)(w + 0x58) = zero;
        *(u32 *)(w + 0x5c) = zero;
        Anim_LinkToList(w, (u32) - (flags & 0x80) >> 31);
        return 0;
    }
    return -1;
}
