#include "global.h"
extern u32 gUnk_02000610[];
u32 sub_0821A8A4(u32, u32);
void sub_0821A8C8(u32);
u8 *sub_0821A66C(u8 *, u32 *);
u32 sub_0821AC6C(u8 *);
u32 sub_0821AD2C(u8 *);
u32 sub_0821BA5C(u8 *);
u32 sub_0821AF2C(u8 *p, u32 a, u32 b)
{
    u32 length, saved;
    saved = sub_0821A8A4(a, b);
    while (p) {
        switch (*p & 0xF0) {
        case 0x60:
            p = sub_0821A66C(p, &length);
            if (sub_0821AC6C(p) == 1) {
                p = (u8 *)1;
                goto done;
            }
            p += length;
            break;
        case 0x70:
            p = sub_0821A66C(p, &length);
            gUnk_02000610[1] = sub_0821AD2C(p);
            p += length;
            break;
        case 0x30:
            p = sub_0821A66C(p, &length);
            gUnk_02000610[1] = sub_0821BA5C(p);
            p += length;
            break;
        case 0:
            goto stop;
        }
    }
stop:
    p = 0;
done:
    sub_0821A8C8(saved);
    return (u32)p;
}
