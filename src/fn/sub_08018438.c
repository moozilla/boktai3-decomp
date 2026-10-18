#include "global.h"
void PaletteCycle_RefreshBasePalette(u8 *p)
{
    CpuSet((void *)0x03004FC0, p + 0x1c, 0x04000080);
}
