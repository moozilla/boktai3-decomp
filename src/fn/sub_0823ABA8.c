#include "global.h"

bool32 sub_0823ABA8(u8 *p, s32 value)
{
    if (*(u16 *)(p + 0x428) < value)
        return FALSE;
    return TRUE;
}
