#include "global.h"
struct Rect { s16 x0, z0, x1, z1; u8 y0, y1; u16 id; };
struct Ref { u8 bytes[4]; u16 first, index; };
u8 *Script_GetPc(void);
u32 Script_GetValue(void);
struct Rect *sub_0821E104(u16, u16 *);
void sub_0821AAD8(struct Ref *);
void sub_0821B4C8(struct Ref *, s32, s32);
s32 sub_0821E52C(u16 id, s16 *p)
{
    u16 count, i;
    struct Rect *r = sub_0821E104(id, &count);
    if (!r) return 0;
    for (i = 0; i < count; r++, i++) {
        if (p[0] >= r->x0 && p[0] < r->x1 && p[2] >= r->z0 && p[2] < r->z1)
            return 1;
    }
    return 0;
}
