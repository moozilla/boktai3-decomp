#include "stage_icon_display.h"
#define StageIconDisplay_CreateFromScript sub_08056020
struct StageIconDisplay *StageIconDisplay_CreateFromScript(void)
{
    if (gUnk_0200011C)
        return gUnk_0200011C;
    {
        struct StageIconDisplay *display = Task_Create(4, sizeof(struct StageIconDisplay));
        if (display) {
            Task_SetCallbacks(display, (void (*)(void *))sub_08055D74,
                              (void (*)(void *))sub_08055DA0);
            if (sub_08055FA4(display) < 0) {
                Task_Kill(display);
                return 0;
            }
        }
        return display;
    }
}
