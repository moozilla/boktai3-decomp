#include "global.h"
void Vorn_ResetStateEntry(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
}
