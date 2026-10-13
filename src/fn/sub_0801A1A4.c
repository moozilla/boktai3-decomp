#include "global.h"
extern u16 gUnk_03005260[];
void sub_0821980C(u8 *, u8 *, int, int);
void sub_0822B2F8(int);
void SelectionMenu_HandleHorizontalInput(u8 *p)
{
    if ((gUnk_03005260[1] & 0x20) && p[0x20] == 1) {
        p[0x20] = 0;
        sub_0821980C(p + 0x254, p + 0x4c, 0x4b, 0);
        sub_0822B2F8(0xdc);
    }
    if ((gUnk_03005260[1] & 0x10) && p[0x20] == 0) {
        p[0x20] = 1;
        sub_0821980C(p + 0x254, p + 0x4c, 0x4c, 0);
        sub_0822B2F8(0xdc);
    }
}
