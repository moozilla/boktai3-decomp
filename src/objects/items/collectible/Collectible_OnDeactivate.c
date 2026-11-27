#include "global.h"
void Collectible_OnDeactivate(u8 *p)
{
    if (p[0xf1]) {
        p[0xf0] = 0;
        p[0xf1] = 0;
    }
}
