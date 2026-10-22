#include "global.h"
extern u8 *gUnk_02000580[];

bool32 sub_080D6FBC(void)
{
    u8 *player = gUnk_02000580[0];
    u32 mask = 8;
    if (*(u32 *)(player + 0x20) & mask)
        return TRUE;
    return FALSE;
}
