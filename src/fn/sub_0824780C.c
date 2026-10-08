#include "global.h"
extern u8 *gUnk_03006A80; extern u8 *gUnk_03006A84; void sub_0824784C(void); void sub_08244150(void (*)(void)); void sub_082444EC(void);
void sub_0824780C(void) { u8 *g = gUnk_03006A80; if (g[0] != 0xff) { u8 *a = gUnk_03006A84; u32 v = g[4] | g[5] | g[6]; u32 z = 0; a[3] = v; gUnk_03006A84[4] = z; sub_08244150(sub_0824784C); sub_082444EC(); } }
