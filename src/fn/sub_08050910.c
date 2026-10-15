#include "orbit_background.h"
// MATCHING_CONCESSION: register-pinning
#define OrbitBackground_UpdateVisibility sub_08050910
static inline u32 isSpecial(void)
{
    u32 value = gUnk_02000528;
    u32 result = 0;
    if (value == 1)
        result = 1;
    return result;
}
void OrbitBackground_UpdateVisibility(struct OrbitBackground *input)
{
    register struct OrbitBackground *background asm("r5") = input;
    if (gUnk_030053F4 & 0x800) {
        register s32 selected asm("r1");
        u32 status;
        if (!gUnk_020004B4)
            selected = -1;
        else
            selected = gUnk_020004B4->selected;
        if (!gUnk_020004B4 || selected < 0)
            status = 0;
        else
            status = gUnk_020004B4->status[selected];
        asm("" : "+r"(status));
        if (status)
            background->enabled = 0;
        else if (background->enabled)
            goto enable;
        else {
            sub_080508F0(background);
            background->enabled = 1;
        }
        if (background->enabled)
            goto enable;
        goto disable;
    } else {
        u32 special = isSpecial();
        u8 *saved;
        if (special) {
            u8 *mode = &background->specialMode;
            u32 value = *mode;
            saved = mode;
            if (value == 0)
                *saved = 1;
        } else {
            u8 *mode = &background->specialMode;
            u32 value = *mode;
            saved = mode;
            if (value != 0) {
                *saved = special;
                if (!background->enabled)
                    goto disable;
                sub_080508F0(background);
            }
        }
        if (!background->enabled)
            goto disable;
        if (*saved)
            goto disable;
    }
enable: {
    u32 mask = 512;
    gUnk_03004BD8 = mask | gUnk_03004BD8;
}
    return;
disable: {
    u16 *control = &gUnk_03004BD8;
    u32 mask = ~512;
    *control = mask & *control;
}
}
