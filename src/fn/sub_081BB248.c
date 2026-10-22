#include "global.h"
extern u16 gUnk_03005260[];

void sub_081BB248(u16 *cursor)
{
    if (gUnk_03005260[0] & 0x10)
        cursor[0]++;
    else if (gUnk_03005260[0] & 0x20)
        cursor[0]--;
    if (gUnk_03005260[0] & 0x40)
        cursor[1]--;
    else if (gUnk_03005260[0] & 0x80)
        cursor[1]++;
}
