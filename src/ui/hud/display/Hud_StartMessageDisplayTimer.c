#include "global.h"
#include "engine/ram.h"
void Hud_StartMessageDisplayTimer(void)
{
    u8 *g = gFieldHud;
    if (g != 0)
        g[0xa6d] = 0x5a;
}
