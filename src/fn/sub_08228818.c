#include "global.h"
s32 Mod(s32, s32); s32 Div(s32, s32);
u8 sub_08228818(u8 a) { s32 r = Mod(a, 10); return (Div(Mod(a, 100) - r, 10) << 4) | r; }
