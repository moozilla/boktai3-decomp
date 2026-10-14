#include "background_flash.h"
void BackgroundFlash_SetState(struct BackgroundFlashController *controller, u32 state)
{
    controller->state = state;
    controller->enter = 1;
    controller->stateFrame = 0;
}
