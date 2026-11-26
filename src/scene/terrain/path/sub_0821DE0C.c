#include "global.h"
void Trap_FindBySerial(u32);
void sub_0821DE0C(u8 *p)
{
    s32 i;
    u32 *serial;
    for (i = 0; i < p[3]; i++) {
        serial = (u32 *)(p + 8);
        Trap_FindBySerial(serial[i]);
    }
}