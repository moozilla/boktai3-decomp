#include "global.h"
#include "ram.h"

u32 TextWindowManager_FindWindow(u32, u32);
u32 TextWindow_Find(u32 a)
{
    u32 g = gWindowManager;
    if (g == 0)
        return 0;
    return TextWindowManager_FindWindow(g, a);
}
