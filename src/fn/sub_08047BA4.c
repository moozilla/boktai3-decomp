#include "background_flash.h"
#define BackgroundFlash_GetState sub_08047BA4
u8 BackgroundFlash_GetState(struct BackgroundFlashController *controller)
{
    if (!controller)
        return 0;
    return controller->state;
}
