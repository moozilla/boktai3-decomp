#include "global.h"
void PaletteFrames_CaptureColors(u8 *p)
{
    CpuSet((void *)0x03004FC0, p + 0x20, 0x04000080);
}
