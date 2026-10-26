#include "global.h"
#include "ram.h"

void sub_082194D4(void);

u32 ScanlineWave_Destroy(void)
{
    sub_082194D4();
    gScanlineWave = 0;
    return 0;
}
