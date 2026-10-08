#include "global.h"
extern u8 *gUnk_030053F8; u32 sub_0822ED64(u32); s32 sub_0822F10C(u32, void *, u32); void sub_08219DD8(void *, u32);
u32 sub_0822BBA8(void) { u32 r; u32 n = sub_0822ED64(0x28); if (sub_0822F10C(0, gUnk_030053F8, n) >= 0) r = 1; else { sub_08219DD8(gUnk_030053F8, 0x28); r = 0; } return r; }
