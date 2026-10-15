#include "shake_icon_display.h"
#define ShakeIconDisplay_CreateFromScript sub_08056EE4
struct ShakeIconDisplay *ShakeIconDisplay_CreateFromScript(void)
{
    struct ShakeIconDisplay *display = Task_Create(9, sizeof(struct ShakeIconDisplay));
    if (display) {
        Task_SetCallbacks(display, (void (*)(void *))sub_08056DBC, (void (*)(void *))sub_08056DFC);
        if (sub_08056E0C(display) != 0) {
            Task_Kill(display);
            return 0;
        }
    }
    return display;
}
