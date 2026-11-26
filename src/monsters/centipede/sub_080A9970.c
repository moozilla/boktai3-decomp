#include "global.h"
void TentacleActor_InitSegmentUpdateState(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3d0);
    q[0x874] = 1;
}
