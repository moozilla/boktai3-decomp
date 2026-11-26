#include "engine/shared/global.h"
#include "effects/background_controllers/background_controller.h"

void Background_ClearSlots(void)
{
    s32 i;
    for (i = 0; i < NUM_BACKGROUNDS; i++)
        gBackgrounds[i] = NULL;
}
