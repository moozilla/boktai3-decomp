#include "palette_frames.h"

void PaletteFrames_CaptureColors(struct PaletteFrames *cycle)
{
    CpuSet(gBgPaletteBuffer, cycle->colors, 0x04000080);
}

int PaletteFrames_Update(struct PaletteFrames *cycle)
{
    int i;
    if (Palette_IsDirty())
        PaletteFrames_CaptureColors(cycle);
    if (++cycle->time > 3) {
        cycle->time = 0;
        if (++cycle->frame > 6)
            cycle->frame = 0;
        for (i = 0; i < 16; i++)
            gBgPaletteBuffer[i] = cycle->colors[cycle->frame * 16 + i];
    }
    return 0;
}

u32 PaletteFrames_Destroy(void)
{
    return 0;
}

s32 PaletteFrames_Initialize(struct PaletteFrames *cycle)
{
    PaletteFrames_CaptureColors(cycle);
    return 0;
}

struct PaletteFrames *PaletteFrames_Create(void)
{
    struct PaletteFrames *cycle = Task_Create(12, sizeof(*cycle));
    if (cycle) {
        Task_SetCallbacks(cycle, (void *)PaletteFrames_Update, (void *)PaletteFrames_Destroy);
        if (PaletteFrames_Initialize(cycle) < 0) {
            Task_Kill(cycle);
            return 0;
        }
    }
    return cycle;
}
