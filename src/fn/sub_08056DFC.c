#include "shake_icon_display.h"
u32 ShakeIconDisplay_Destroy(struct ShakeIconDisplay *display)
{
    Anim_Remove(&display->animation);
    return 0;
}
