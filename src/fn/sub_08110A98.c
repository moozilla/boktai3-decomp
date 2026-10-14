#include "global.h"
#include "engine.h"

extern void *gUnk_020001D4;
void sub_0810FD90(void);
void sub_08033468(void);

void sub_08110A98(void)
{
    if (gUnk_020001D4) {
        sub_0810FD90();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        sub_08033468();
        Task_Kill(gUnk_020001D4);
    }
}
