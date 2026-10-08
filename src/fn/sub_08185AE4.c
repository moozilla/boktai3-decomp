#include "global.h"
struct S { u8 f0[0x48]; s16 a48; s16 a4a; s16 a4c; u8 g0[0xB3A-0x4E]; u8 b3a; u8 f1; u8 b3c; u8 b3d; u8 b3e; u8 b3f; u8 b40; u8 f2[4]; u8 b45; u8 g1[0x1124-0xB46]; s16 x; s16 y; s16 z;};
void sub_081850B4(struct S *, s32);
void sub_081851F8(struct S *, s32);
void sub_08185AE4(struct S *p)
{
    switch (p->b3d) {
    case 0:
        sub_081850B4(p, 0x10);
        p->b3d = 1;
        break;
    case 1:
        if (p->b40 == 0xff) sub_081851F8(p, 0xb);
        break;
    }
}
