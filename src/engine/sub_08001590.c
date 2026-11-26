#include "global.h"
#include "effects/screen/background_controller.h"

void Background_ClearSlots(void)
{
    s32 i;
    for (i = 0; i < NUM_BACKGROUNDS; i++)
        gBackgrounds[i] = NULL;
}
