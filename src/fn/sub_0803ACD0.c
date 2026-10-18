// MATCHING_CONCESSION: register-pinning
#include "global.h"
int WirelessGame_GetRemotePlayerHealthPercent(void);
int WirelessGame_GetRemoteOpponentHealthPercent(void);
void Anim_LoadKeyframe(void *, void *, int, int);
void WirelessGameDisplay_UpdateStatusIcons(u8 *p)
{
    int value = WirelessGame_GetRemotePlayerHealthPercent();
    int category = 0;
    if (value <= 60) {
        category = 2;
        if (value > 25)
            category = 1;
    }
    {
        register int offset asm("r3");
        void *first = p + 480;
        void *descriptor;
        offset = 448;
        descriptor = p + offset;
        offset -= 43;
        Anim_LoadKeyframe(first, descriptor, category + offset, 0);
    }
    {
        int category;
        value = WirelessGame_GetRemoteOpponentHealthPercent();
        category = 0;
        if (value <= 60) {
            category = 2;
            if (value > 25)
                category = 1;
        }
        {
            int initialOffset = 576;
            void *first;
            void *descriptor;
            register int base asm("r4");
            register int value asm("r2");
            first = p + initialOffset;
            value = 448;
            asm("" : "+r"(value));
            descriptor = p + value;
            base = 408;
            asm("" : "+r"(base));
            value = base;
            asm("" : "+r"(value));
            category |= value;
            Anim_LoadKeyframe(first, descriptor, category, 0);
        }
    }
}
