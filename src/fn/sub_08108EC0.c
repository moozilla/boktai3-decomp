#include "global.h"

void LinkConnection_SetRetryOptions(u8 *p, u32 a)
{
    *(u32 *)(p + 0x70) = 0;
    if (a & 1) {
        *(u32 *)(p + 0x70) = 1;
    }
    if (a & 4) {
        *(u16 *)(p + 0x6A) = 0x1E;
    } else {
        *(u16 *)(p + 0x6A) = 0x78;
    }
}
