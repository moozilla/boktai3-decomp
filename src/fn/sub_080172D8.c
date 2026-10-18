#include "global.h"

extern u8 *gUnk_0200007C;
void sub_082194D4(void);

u32 ScanlineWave_Destroy(void)
{
    sub_082194D4();
    gUnk_0200007C = 0;
    return 0;
}
