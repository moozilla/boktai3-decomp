#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_SelectTimePalette(struct PileDriver *input, s32 phase)
{
    struct PileDriverEffectsView *panel = (void *)input;
    switch (panel->timePhase) {
        case 0:
            panel->previousPalette = 2;
            panel->targetPalette = 2;
            break;
        case 1:
            panel->previousPalette = 2;
            panel->targetPalette = 0;
            panel->paletteBlendActive = 1;
            panel->paletteBlendFrame = 0;
            break;
        case 2:
            panel->previousPalette = 0;
            panel->targetPalette = 0;
            break;
        case 3:
            panel->previousPalette = 0;
            panel->targetPalette = 1;
            panel->paletteBlendActive = 1;
            panel->paletteBlendFrame = 0;
            break;
        case 4:
            panel->previousPalette = 1;
            panel->targetPalette = 2;
            panel->paletteBlendActive = 1;
            panel->paletteBlendFrame = 0;
            break;
        case 5:
            panel->previousPalette = 2;
            panel->targetPalette = 2;
            break;
    }
}
