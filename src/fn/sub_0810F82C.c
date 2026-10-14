#include "global.h"
#include "engine.h"

extern void *gUnk_020001D0;
void sub_0810F1FC(void);
void sub_08033468(void);

void sub_0810F82C(void)
{
    if (gUnk_020001D0) {
        sub_0810F1FC();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        sub_08033468();
        Task_Kill(gUnk_020001D0);
    }
}
