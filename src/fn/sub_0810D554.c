#include "global.h"
#include "engine.h"

extern void *gUnk_020001C8;
void sub_0810D14C(void);
void sub_08033468(void);

void sub_0810D554(void)
{
    if (gUnk_020001C8) {
        sub_0810D14C();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        sub_08033468();
        Task_Kill(gUnk_020001C8);
    }
}
