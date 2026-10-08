#include "global.h"
extern const u8 gUnk_08602D2C[], gUnk_08602D38[]; extern const u8 *gUnk_03006A9C;
u32 sub_082485EC(u16 x) { u32 r = 0; if (x == 4) gUnk_03006A9C = gUnk_08602D2C; else if (x == 0x40) gUnk_03006A9C = gUnk_08602D38; else { gUnk_03006A9C = gUnk_08602D2C; r = 1; } return r; }
