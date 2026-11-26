#ifndef GUARD_PILE_DRIVER_LIFECYCLE_H
#define GUARD_PILE_DRIVER_LIFECYCLE_H
#include "objects/pile_driver/pile_driver_commands.h"

extern void (*const gUnk_08605ABC[])(struct PileDriver *);
void PileDriver_ReadInitialMode(struct PileDriver *);
void PileDriver_InitAnchorPositions(struct PileDriver *);
void PileDriver_CreateSubjects(struct PileDriver *);
void PileDriver_InitScriptProperty(struct PileDriver *);
void PileDriver_UpdateParticles(struct PileDriver *);
void PileDriver_DestroyParticles(struct PileDriver *);
void PileDriver_InitParticles(struct PileDriver *);
void PileDriver_InitTimePalette(struct PileDriver *);
void PileDriver_UpdatePaletteState(struct PileDriver *);
void PileDriver_DrawBackgroundLayers(struct PileDriver *);
void PileDriver_NoOpAction(struct PileDriver *);
void PileDriver_InitializeInteraction(struct PileDriver *);
#endif
