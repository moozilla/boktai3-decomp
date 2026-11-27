#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_UpdatePaletteState(struct PileDriver *input)
{
    struct PileDriverEffectsView *panel = (void *)input;
    if (panel->state)
        panel->state(input);
    PileDriver_UpdateTimePalette(panel);
    if (panel->channelsDirty) {
        s32 i;
        for (i = 0; i < 3; i++)
            CpuSet(panel->paletteColors[panel->channels[i]], gBgPaletteBuffer + i * 16, 0x04000008);
        panel->channelsDirty = 0;
    }
}
