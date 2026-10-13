#include "global.h"

extern void *gUnk_020001C4;
void sub_0810C14C(void);
void sub_08033468(void);
void sub_080335B4(void);
void Bg_ClearTilemap(s32);
void Task_Kill(void *);

void sub_0810CB88(void)
{
    if (gUnk_020001C4 != 0) {
        sub_0810C14C();
        sub_08033468();
        sub_080335B4();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        Task_Kill(gUnk_020001C4);
    }
}
