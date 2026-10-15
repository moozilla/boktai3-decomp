#include "proximity_icon.h"
struct ProximityIcon *ProximityIcon_CreateFromScript(void)
{
    struct ProximityIcon *icon = Task_Create(9, sizeof(struct ProximityIcon));
    if (icon) {
        Task_SetCallbacks(icon, (void (*)(void *))ProximityIcon_Update,
                          (void (*)(void *))ProximityIcon_Destroy);
        if (ProximityIcon_InitFromScript(icon) < 0) {
            Task_Kill(icon);
            return 0;
        }
    }
    return icon;
}
