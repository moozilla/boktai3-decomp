#include "global.h"
struct S { u8 f0[0x48]; s16 a48; s16 a4a; s16 a4c; u8 g0[0xB39-0x4E]; u8 b39; u8 b3a; u8 f1; u8 b3c; u8 b3d; u8 b3e; u8 b3f; u8 b40; u8 b41; u8 b42; u8 b43; u8 f2; u8 b45; u8 g1[0x1124-0xB46]; s16 x; s16 y; s16 z;};
s32 sub_0818505C(struct S *, s32);
void sub_0818E9B8(void *, s32);
void sub_0818591C(struct S *p)
{
    u8 s = p->b3d;
    switch (s) {
    case 0:
        if (sub_0818505C(p, 0xa)) { p->b3d = 1; p->b3e = s; }
        break;
    case 1:
        if (p->b40 == 0xff) {
            sub_0818E9B8((u8 *)p + 0xB58, 0x64);
            if (p->b3e > 0x13) {
                if (sub_0818505C(p, 0xc)) p->b3d = 3;
            }
        }
        break;
    case 3:
        if (p->b40 == 0xff) p->b3d = 0xff;
        break;
    }
}
