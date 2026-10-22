#include "global.h"

u16 *sub_082196FC(u8 *table, u16 id)
{
    u16 *entry = *(u16 **)(table + 0xc);
    s32 i;
    for (i = 0; i < *(u16 *)(table + 2); i++) {
        if (*entry == id)
            return entry;
        entry += 10;
    }
    return NULL;
}
