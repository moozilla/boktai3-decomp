#include "global.h"
struct Rect { s16 x0, z0, x1, z1; u8 y0, y1; u16 id; };
struct Ref { u8 bytes[4]; u16 first, index; };
u8 *Script_GetPc(void);
u32 Script_GetValue(void);
struct Rect *sub_0821E104(u16, u16 *);
void sub_0821AAD8(struct Ref *);
void sub_0821B4C8(struct Ref *, s32, s32);
static inline s32 half(s32 value)
{
    if (value >= 0) return value >> 1;
    return -((-value) >> 1);
}
static inline s32 half_shift(s32 value)
{
    if (value >= 0) value >>= 1;
    else value = -((-value) >> 1);
    return value << 4;
}
s32 sub_0821E3C0(void)
{
    u16 count;
    struct Ref ref;
    struct Rect *r;
    s32 value, sum;
    u8 *write;
    if (!Script_GetPc()) return -1;
    r = sub_0821E104(Script_GetValue(), &count);
    if (!r) return -1;
    sub_0821AAD8(&ref);
    sub_0821B4C8(&ref, 0, r->x0);
    sub_0821AAD8(&ref);
    sub_0821B4C8(&ref, 0, r->y0 << 4);
    sub_0821AAD8(&ref);
    sub_0821B4C8(&ref, 0, r->z0);
    sub_0821AAD8(&ref);
    sub_0821B4C8(&ref, 0, r->x1);
    sub_0821AAD8(&ref);
    sub_0821B4C8(&ref, 0, r->y1 << 4);
    sub_0821AAD8(&ref);
    sub_0821B4C8(&ref, 0, r->z1);
    sub_0821AAD8(&ref);
    write = (u8 *)&ref;
    sum = r->x1 + r->x0;
    value = half(sum);
    sub_0821B4C8((struct Ref *)write, 0, value);
    sub_0821AAD8(&ref);
    write = (u8 *)&ref;
    sum = r->y1 + r->y0;
    sub_0821B4C8((struct Ref *)write, 0, half_shift(sum));
    sub_0821AAD8(&ref);
    write = (u8 *)&ref;
    sum = r->z1 + r->z0;
    value = half(sum);
    sub_0821B4C8((struct Ref *)write, 0, value);
    return 0;
}
