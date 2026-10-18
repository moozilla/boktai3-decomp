#include "global.h"
u32 Modifier_UpdateIdleDrain(u8 *p)
{
    *(u8 *)(p + 0xbf) = 0;
    *(s32 *)(p + 0xc0) += -0x140;
    return 0;
}
