#include "shake_icon_display.h"
void ShakeIconDisplay_TickShake(struct ShakeIconDisplay *display)
{
    if (display->stateFrame <= 5) {
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            s32 sample = *(u16 *)(offset + RANDOM_TABLE_ADDR);
            display->animation.position.x = ((sample >> 4) & 15) - 7;
        }
    } else {
        display->animation.position.x = 0;
        display->animation.position.y = 0;
    }
}
