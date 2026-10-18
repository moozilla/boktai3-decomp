#include "global.h"

extern u32 gUnk_020000E4;
u32 TextWindowManager_FindWindow(u32, u32);
u32 TextWindow_Find(u32 a)
{
    u32 g = gUnk_020000E4;
    if (g == 0)
        return 0;
    return TextWindowManager_FindWindow(g, a);
}
