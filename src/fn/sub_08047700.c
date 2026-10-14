#include "background_flash.h"
#define BackgroundFlash_TakeEntryFlag sub_08047700
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
