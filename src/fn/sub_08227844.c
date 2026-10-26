#include "global.h"
#include "engine.h"

extern u32 gUnk_030053F0, gUnk_030053EC, gUnk_03005410, gUnk_030053DC, gUnk_03005414;

void Engine_ClearControlFlags(void)
{
    gEngineFlags = 0;
    gUnk_030053F0 = 0;
    gUnk_030053EC = 0;
    gUnk_03005410 = 0;
    gUnk_030053DC = 0;
    gUnk_03005414 = 0;
}
