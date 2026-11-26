#ifndef GUARD_PILE_DRIVER_ORCHESTRATION_H
#define GUARD_PILE_DRIVER_ORCHESTRATION_H
#include "objects/pile_driver/pile_driver_interaction.h"
#include "system/save/script_reference.h"
#include "objects/pile_driver/pile_driver_config.h"

#define PILE_DRIVER_GLOBAL ((struct PileDriver *)gPileDriver)
void PileDriver_InitRig(struct PileDriver *);
void PileDriver_InitBackground(struct PileDriver *);
void PileDriver_InitOverlay(struct PileDriver *);
#endif
