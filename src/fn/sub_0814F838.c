#include "global.h"

void Script_Call(s32, s32);

void sub_0814F838(u8 *p)
{
    u8 *slot = p + 0x5A4;
    s32 v = *(s32 *)slot;

    if (v != 0) {
        *(s32 *)slot = 0;
        Script_Call(v, 0);
    }
}
