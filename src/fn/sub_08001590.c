#include "global.h"
#include "background_controller.h"

void Background_ClearSlots(void)
{
    s32 i;
    for (i = 0; i < NUM_BACKGROUNDS; i++)
        gBackgrounds[i] = NULL;
}
