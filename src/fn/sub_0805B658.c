#include "global.h"
extern u32 gUnk_02000580;
s32 sub_0805B318(u8 *);
void sub_0815C99C(u32);
void sub_08057960(u8 *, u32);
void sub_08057978(u8 *, u32);
void sub_0805B658(u8 *p)
{
    if (sub_0805B318(p) != 0) {
        sub_0815C99C(gUnk_02000580);
        sub_08057960(p, 3);
        sub_08057978(p, 0);
    }
}
