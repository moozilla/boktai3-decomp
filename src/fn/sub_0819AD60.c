#include "global.h"

void sub_08180638(u8 *);

u32 SceneOrbitHazard_OnDestroy(u8 *p)
{
    sub_08180638(p + 0x18);
    return 0;
}
