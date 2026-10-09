#include "global.h"
extern u8 *gUnk_03002604;
extern u8 *gUnk_02000710;
extern u16 gUnk_030054C0, gUnk_030054C4, gUnk_030054BC;
void sub_0822C110(void)
{
    gUnk_03002604 = 0;
    *(u16 *)(gUnk_02000710 + 0x874) = 0;
    *(u16 *)(gUnk_02000710 + 0x876) = 0;
    gUnk_030054C0 = 0;
    gUnk_030054C4 = 0;
    gUnk_030054BC = 0;
}
