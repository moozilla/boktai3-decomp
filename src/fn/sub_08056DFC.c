#include "shake_icon_display.h"
#define ShakeIconDisplay_Destroy sub_08056DFC
u32 ShakeIconDisplay_Destroy(struct ShakeIconDisplay *display)
{
    Anim_Remove(&display->animation);
    return 0;
}
