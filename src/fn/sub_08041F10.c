#include "global.h"

void sub_080414A0(u8 *);
void Bg_ClearTilemap(u32);
void sub_08040FE4(u8 *);

void sub_08041F10(u8 *p)
{
    sub_080414A0(p);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    sub_08040FE4(p);
}
