#include "global.h"
#ifdef NONMATCHING
// fuzzy: 95.8%
// foundry draft (worker best 95.8%); what differs is mostly register allocation and instruction order
#define MenuSelection_ProcessOptionInput sub_08175C40
extern u16 gUnk_03005260[];
void Sound_PlaySong(u32);
void MenuSelection_RefreshOptionText(u8 *);
s32 sub_08175C40(u8 *s)
{
    u32 keys = gUnk_03005260[1];
    u16 up;
    u8 *row;
    if (keys & 0x10) {
        row = s + 0x48C0;
        if (*row > 1)
            goto toggle;
        {
            u8 *option = s + 0x48C1;
            option += *row;
            if (*option > 1)
                *option = 0;
            else
                *option += 1;
            goto changed;
        }
    }
    if (!(keys & 0x20))
        goto vertical;
    row = s + 0x48C0;
    if (*row <= 1) {
        {
            u8 *option = s + 0x48C1;
            option += *row;
            if (*option == 0)
                *option = 2;
            else
                *option -= 1;
            goto changed;
        }
    }
toggle: {
    u8 *option = s + 0x48C1;
    option += *row;
    *option = 1 - *option;
}
changed:
    Sound_PlaySong(0x19A);
    MenuSelection_RefreshOptionText(s);
    return 1;
vertical:
    up = 0x40;
    up &= keys;
    if (up) {
        row = s + 0x48C0;
        if (*row == 0)
            *row = 2;
        else
            *row -= 1;
    } else if (0x80 & keys) {
        row = s + 0x48C0;
        if (*row == 2)
            *row = up;
        else
            *row += 1;
    } else
        goto none;
    Sound_PlaySong(0xDC);
    return 1;
none:
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08175C40);
#endif
