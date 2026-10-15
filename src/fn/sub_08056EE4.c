#include "shake_icon_display.h"
struct ShakeIconDisplay *ShakeIconDisplay_CreateFromScript(void)
{
    struct ShakeIconDisplay *display = Task_Create(9, sizeof(struct ShakeIconDisplay));
    if (display) {
        Task_SetCallbacks(display, (void (*)(void *))ShakeIconDisplay_Tick,
                          (void (*)(void *))ShakeIconDisplay_Destroy);
        if (ShakeIconDisplay_Initialize(display) != 0) {
            Task_Kill(display);
            return 0;
        }
    }
    return display;
}
