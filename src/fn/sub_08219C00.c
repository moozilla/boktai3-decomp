#include "global.h"
#include "runtime.h"

u32 Random_Next15(void)
{
    gLcgState = gLcgState * 0x5D588B65 + 1;
    return gLcgState & 0x7FFF;
}
