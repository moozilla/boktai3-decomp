#include "global.h"

void Vanargand_StartActionSequence(u8 *p)
{
    if (p[0x15] == 0)
        p[0x15] = 1;
}
