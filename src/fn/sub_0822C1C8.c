#include "global.h"
struct S { u8 p[0x19]; u8 b; };
extern struct S *gUnk_03002604; extern u16 gUnk_030054BC;
void sub_08243E8C(void);
void sub_0822C1C8(void) { if (gUnk_03002604) { if (gUnk_03002604->b) sub_08243E8C(); gUnk_030054BC = 1; } }
