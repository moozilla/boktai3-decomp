#include "global.h"
void rfu_clearAllSlot(void);

bool32 sub_08035CD0(u8 *p)
{
    if (p[0x30] != 0) {
        rfu_clearAllSlot();
        return TRUE;
    }
    return FALSE;
}
