#include "global.h"
extern u8 *gUnk_03006A88, *gUnk_03006A84; void sub_08245F6C(void); void sub_08244150(void (*)(void)); void sub_08244438(void);
void sub_082460C0(void) { u8 *p; sub_08244150(sub_08245F6C); sub_08244438(); p = *(u8 **)(gUnk_03006A88 + 0xdc); if (p[6] <= 3) { u8 *q = gUnk_03006A84 + 0xa; q[p[6]] = 0; } }
