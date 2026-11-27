#include "objects/pile_driver/pile_driver.h"
void PileDriver_SetState(struct PileDriver *panel, void (*state)(struct PileDriver *))
{
    panel->state = state;
    panel->stateFrame = 0;
}
