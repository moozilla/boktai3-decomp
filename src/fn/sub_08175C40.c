#include "global.h"
#include "input.h"
// MATCHING_CONCESSION: register-pinning
void Sound_PlaySong(u32);
void MenuSelection_RefreshOptionText(u8 *);
s32 MenuSelection_ProcessOptionInput(u8 *s)
{
    u32 keys = gPadState[1];
    u16 up;
    u32 m;
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
    m = 0x40;
    up = keys & m;
    if (up) {
        row = s + 0x48C0;
        if (*row == 0)
            *row = 2;
        else
            *row -= 1;
    } else {
        m = 0x80;
        asm("" : "+r"(m));
        if (m & keys) {
            row = s + 0x48C0;
            if (*row == 2)
                *row = up;
            else
                *row += 1;
        } else
            goto none;
    }
    Sound_PlaySong(0xDC);
    return 1;
none:
    return 0;
}
