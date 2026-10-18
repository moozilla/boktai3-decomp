#include "global.h"

void Script_Call(u32, void *);

void Bat_TickDelayedScript(u8 *p)
{
    s32 k2 = 0; // a zero held in a register from the start (found by tools/autosolve.py)
    u8 *q = *(u8 **)(p + 0x3D0);
    u16 *script = (u16 *)(q + 0x624);
    u8 *count;
    u8 *limit;

    if (*script != 0) {
        count = q + 0x627;
        *count += 1;
        limit = q + 0x626;
        if (*count >= *limit) {
            u8 z;
            Script_Call(*script, k2);
            z = 0;
            *script = 0;
            *limit = z;
            *count = z;
        }
    }
}
