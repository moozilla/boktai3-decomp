#include "projected_display.h"
struct ProjectedDisplay *ProjectedDisplay_Create(const struct ContactPosition *position)
{
    struct ProjectedDisplay *display = Task_Create(10, sizeof(*display));
    if (display) {
        Task_SetCallbacks(display, (void (*)(void *))ProjectedDisplay_Update,
                          (void (*)(void *))ProjectedDisplay_Destroy);
        if (ProjectedDisplay_Initialize(display, position) < 0) {
            Task_Kill(display);
            return 0;
        }
    }
    return display;
}
