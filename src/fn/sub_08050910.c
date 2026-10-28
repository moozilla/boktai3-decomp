#include "orbit_background.h"
// MATCHING_CONCESSION: register-pinning
static inline u32 isSpecial(void)
{
    u32 value = gPlayerActionPending;
    u32 result = 0;
    if (value == 1)
        result = 1;
    return result;
}
void OrbitBackground_UpdateVisibility(struct OrbitBackground *input)
{
    register struct OrbitBackground *background asm("r5") = input;
    if (gEngineFlags & 0x800) {
        register s32 selected asm("r1");
        u32 status;
        if (!gLinkSession)
            selected = -1;
        else
            selected = gLinkSession->selected;
        if (!gLinkSession || selected < 0)
            status = 0;
        else
            status = gLinkSession->status[selected];
        asm("" : "+r"(status));
        if (status)
            background->enabled = 0;
        else if (background->enabled)
            goto enable;
        else {
            OrbitBackground_ResetOrbit(background);
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
                OrbitBackground_ResetOrbit(background);
            }
        }
        if (!background->enabled)
            goto disable;
        if (*saved)
            goto disable;
    }
enable: {
    u32 mask = 512;
    gDispCntShadow = mask | gDispCntShadow;
}
    return;
disable: {
    u16 *control = &gDispCntShadow;
    u32 mask = ~512;
    *control = mask & *control;
}
}
