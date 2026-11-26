#include "global.h"

void Storage_SetInteractionMode(u8 *p, u8 v)
{
    p[0x174c] = v;
}
