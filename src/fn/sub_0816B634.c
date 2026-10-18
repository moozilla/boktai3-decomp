#include "global.h"
void Hud_SaveSecondaryGridSelection(u8 *p)
{
    p[7] = p[1];
    p[8] = p[2];
    p[9] = p[3];
}
