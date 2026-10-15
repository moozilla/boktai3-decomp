#include "stage_icon_display.h"
struct StageIconDisplay *StageIconDisplay_CreateFromScript(void)
{
    if (gUnk_0200011C)
        return gUnk_0200011C;
    {
        struct StageIconDisplay *display = Task_Create(4, sizeof(struct StageIconDisplay));
        if (display) {
            Task_SetCallbacks(display, (void (*)(void *))StageIconDisplay_Tick,
                              (void (*)(void *))StageIconDisplay_Destroy);
            if (StageIconDisplay_InitFromScript(display) < 0) {
                Task_Kill(display);
                return 0;
            }
        }
        return display;
    }
}
