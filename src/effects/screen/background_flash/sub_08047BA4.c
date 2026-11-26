#include "effects/screen/background_flash.h"
/* BackgroundFlash_GetState: Returns the controller state or zero for a null controller */
u8 BackgroundFlash_GetState(struct BackgroundFlashController *controller)
{
    if (!controller)
        return 0;
    return controller->state;
}
