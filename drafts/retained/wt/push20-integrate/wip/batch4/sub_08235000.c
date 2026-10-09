#include "global.h"
struct Vec50 { s32 x:16, y:16, z:16; };
extern u16 gUnk_03005418[];
extern u32 gUnk_030053F4;
void sub_08234F18(u8 *);
void sub_08234F88(u8 *);
void sub_08234DC8(u8 *);
static inline s32 scale50(s32 v) { if (v >= 0) return v >> 5; return -((-v) >> 5); }
s32 sub_08235000(u8 *p, u8 *owner)
{
    struct Vec50 v;
    u16 *dest;
    s16 *coords;
    s32 x, z, sum, vertical, y, depth;
    u32 zero;
    *(u8 **)(p + 0x18) = owner;
    v = *(struct Vec50 *)(owner + 0x30);
    v.y += 150;
    dest = (u16 *)(p + 0x84);
    coords = (s16 *)&v;
    x = coords[0]; z = coords[2];
    dest[0] = scale50((x-z)*3);
    sum = scale50((x+z)*3);
    vertical = scale50(coords[1]*3);
    zero = 0;
    y = sum - vertical;
    depth = sum + vertical;
    dest[0] = dest[0] - gUnk_03005418[0] + 120;
    dest[1] = y - gUnk_03005418[1] + 80;
    dest[2] = depth - gUnk_03005418[2];
    sub_08234F18(p);
    sub_08234F88(p);
    *(void (**)(u8 *))(p + 0x274) = sub_08234DC8;
    *(u16 *)(p + 0x270) = zero;
    gUnk_030053F4 |= 1;
    return 0;
}
