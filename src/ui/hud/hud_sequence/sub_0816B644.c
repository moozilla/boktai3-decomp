#include "global.h"
void Hud_RestoreSecondaryGridSelection(u8 *p)
{
    p[1] = p[7];
    p[2] = p[8];
    p[3] = p[9];
}
