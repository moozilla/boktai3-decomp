#include "global.h"
extern u8 *gUnk_02000580;
s32 sub_0804B1CC(u8 *);
void sub_08049364(u8 *);
u32 sub_0804B324(u8 *p)
{
    if (p[0x1a] == 9 && p[0x240] != 0) {
        p[0x240] = 0;
        return 0xb;
    }
    if (sub_0804B1CC(p) != 0 && p[0x1a] != 9)
        sub_08049364(p);
    switch (p[0x255]) {
    case 0:
    case 3:
    case 4:
        if (gUnk_02000580[0x457] == 6) goto r1;
        break;
    case 1:
        if (gUnk_02000580[0x457] == 6) {
    r1:
            return 1;
        }
        return 0;
    case 0x10:
        if (gUnk_02000580[0x457] == 6) goto r11;
        break;
    case 0x11:
        if (gUnk_02000580[0x457] == 6) {
    r11:
            return 0x11;
        }
        return 0x10;
    }
    return p[0x255];
}
