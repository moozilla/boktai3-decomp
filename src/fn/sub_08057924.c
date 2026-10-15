#include "proximity_icon.h"
#define ProximityIcon_CreateFromScript sub_08057924
struct ProximityIcon *ProximityIcon_CreateFromScript(void)
{
    struct ProximityIcon *icon = Task_Create(9, sizeof(struct ProximityIcon));
    if (icon) {
        Task_SetCallbacks(icon, (void (*)(void *))sub_080576C8, (void (*)(void *))sub_08057704);
        if (sub_08057714(icon) < 0) {
            Task_Kill(icon);
            return 0;
        }
    }
    return icon;
}
