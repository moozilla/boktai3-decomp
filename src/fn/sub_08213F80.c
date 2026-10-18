#include "global.h"
extern vu32 gUnk_03003A44;
void InterruptsAndVideo_WaitForVBlank(void)
{
    gUnk_03003A44 = 0;
    while (gUnk_03003A44 == 0)
        ;
}
