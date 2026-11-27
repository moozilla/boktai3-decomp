#include "objects/pile_driver/pile_driver.h"
void PileDriver_SetPresentationMode(struct PileDriver *panel)
{
    panel->mode = 0x30;
}
