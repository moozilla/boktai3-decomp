#include "global.h"
void Hud_RestoreGridSelection(u8 *p)
{
    p[1] = p[4];
    p[2] = p[5];
    p[3] = p[6];
}
