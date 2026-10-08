#include "global.h"

s32 Div(s32, s32);
s32 Mod(s32, s32);

void sub_080414C4(u8 *p, u32 n) {
    p[0] = Div(n, 6);
    p[1] = Mod(n, 6);
}
