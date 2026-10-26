#include "global.h"
#include "ram.h"

void Collider_ResetManagerLists(void)
{
    u8 *p = gColliderManager;
    if (p != 0) {
        *(u8 **)(p + 0x18) = p + 0x28;
        *(u8 **)(p + 0x1c) = p + 0x7c;
        *(u32 *)(p + 0x28) = 0;
        *(u32 *)(p + 0x7c) = 0;
        *(u8 **)(p + 0x20) = p + 0x28;
        *(u8 **)(p + 0x24) = p + 0x7c;
        *(u16 *)(p + 0xd0) = 0;
        *(u16 *)(p + 0xd2) = 0;
    }
}
