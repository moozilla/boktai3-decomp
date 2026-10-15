#include "stage_icon_display.h"
#define StageIconDisplay_Create sub_08055F3C
struct StageIconDisplay *
StageIconDisplay_Create(s32 variant, u32 option,
                        void (*completion)(void *, struct StageIconDisplay *), void *argument)
{
    if (gUnk_0200011C)
        return gUnk_0200011C;
    {
        struct StageIconDisplay *display = Task_Create(4, sizeof(struct StageIconDisplay));
        if (display) {
            Task_SetCallbacks(display, (void (*)(void *))sub_08055D74,
                              (void (*)(void *))sub_08055DA0);
            if (sub_08055EEC(display, variant, option, completion, argument) < 0) {
                Task_Kill(display);
                return 0;
            }
        }
        return display;
    }
}
