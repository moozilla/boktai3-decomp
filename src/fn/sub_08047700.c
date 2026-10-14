#include "background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *controller)
{
    u32 result;
    if (!controller->enter) {
        result = 0;
    } else {
        controller->enter = 0;
        result = 1;
    }
    return result;
}
