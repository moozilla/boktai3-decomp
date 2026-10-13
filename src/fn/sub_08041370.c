#include "global.h"

extern u8 *gUnk_02000710;
void sub_0803386C(s32, u32);
void sub_08033A38(s32, u32, u8 *);
void sub_08033924(s32, u32);
void sub_080337FC(s32);
void sub_08041128(u8 *);
void sub_08041280(u8 *);

void sub_08041370(u8 *p)
{
    s32 *q = (s32 *)(p + 0xf0);
    if (*q >= 0) {
        sub_0803386C(*q, *(u32 *)(p + 0xbc));
        sub_08033A38(*q, 0, gUnk_02000710 + 0x790);
        sub_08033924(*q, 3);
        sub_080337FC(*q);
    }
    switch (*(s32 *)(p + 0x18)) {
    case 0:
        sub_08041128(p);
        break;
    case 1:
        sub_08041280(p);
        break;
    case 2:
        sub_08041128(p);
        break;
    }
}
