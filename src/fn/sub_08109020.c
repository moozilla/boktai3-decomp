#include "global.h"
#include "ram.h"

void LinkConnection_SetCallbacks(void *p, u32 a, u32 b)
{
    u8 *g = gSlotMonitor;

    if (g) {
        *(void **)(g + 0x80) = p;
        *(u32 *)(g + 0x74) = a;
        *(u32 *)(g + 0x78) = b;
    }
}
