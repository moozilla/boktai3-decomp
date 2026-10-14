#include "global.h"
#include "engine.h"

extern void *gUnk_020001CC;
void sub_0810DE3C(void);
void sub_08033468(void);

void sub_0810E928(void)
{
    if (gUnk_020001CC) {
        sub_0810DE3C();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        sub_08033468();
        Task_Kill(gUnk_020001CC);
    }
}
