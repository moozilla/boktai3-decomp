#include "global.h"
struct S { u8 f0[0x48]; s16 a48; s16 a4a; s16 a4c; u8 g0[0xB3A-0x4E]; u8 b3a; u8 f1; u8 b3c; u8 b3d; u8 b3e; u8 b3f; u8 b40; u8 f2[4]; u8 b45; u8 g1[0x1124-0xB46]; s16 x; s16 y; s16 z;};
extern const u32 gUnk_0824F014[];
void sub_0824923C(struct S *, u32);
void sub_08186BF8(struct S *p)
{
    const u32 *t = gUnk_0824F014;
    u8 *q = &p->b3c;
    sub_0824923C(p, t[*q]);
    switch (*q) {
    case 0x20: case 0x1b: case 0x1c: case 0x22:
        break;
    default:
        p->b45 = 0;
        break;
    }
    p->b3e++;
}
