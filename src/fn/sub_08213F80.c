#include "global.h"
#include "runtime.h"
void InterruptsAndVideo_WaitForVBlank(void)
{
    gVBlankOccurred = 0;
    while (gVBlankOccurred == 0)
        ;
}
