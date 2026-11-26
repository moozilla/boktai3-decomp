#include "global.h"
void Hud_SaveGridSelection(u8 *p)
{
    p[4] = p[1];
    p[5] = p[2];
    p[6] = p[3];
}
