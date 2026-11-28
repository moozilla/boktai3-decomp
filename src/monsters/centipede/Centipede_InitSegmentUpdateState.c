#include "global.h"
void Centipede_InitSegmentUpdateState(u8 *p)
{
    (*(u8 **)(p + 0x3d0))[0x874] = 1;
}
