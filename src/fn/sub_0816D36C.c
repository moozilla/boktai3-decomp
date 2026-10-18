#include "global.h"
void PauseMenu_RequestEffectMode(u8 *p, u8 v)
{
    p[0x4874] = 1;
    p[0x4875] = v;
    p[0x42A0] = 1;
}
