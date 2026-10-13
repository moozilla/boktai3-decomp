#include "global.h"

void Bg_ClearTilemap(u32);
void Script_Call(u32, u32);

void sub_08109584(u8 *p)
{
    u32 v;

    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    v = *(u32 *)(p + 0x44);
    if (v != 0) {
        Script_Call(v, 0);
    }
}
