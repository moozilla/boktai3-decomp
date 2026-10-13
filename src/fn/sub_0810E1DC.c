#include "global.h"

void sub_08033468(void);
void sub_080335B4(void);
void Bg_ClearTilemap(s32);
void sub_0810DE3C(void *);
void sub_081093AC(s32);
void Task_Kill(void *);

void sub_0810E1DC(void *p)
{
    sub_08033468();
    sub_080335B4();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    sub_0810DE3C(p);
    sub_081093AC(1);
    Task_Kill(p);
}
