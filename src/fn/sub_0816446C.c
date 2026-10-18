#include "global.h"

void Hud_DrawHealthMeter(u32, u32, u32, u32, u32);

void Hud_DrawDefaultHealthMeter(u32 a, u32 b, u32 c, u32 d)
{
    Hud_DrawHealthMeter(a, b, c, d, 0);
}
