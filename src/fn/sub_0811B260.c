#include "global.h"
void Collider_SetOffset(u8 *p, u16 *s)
{
    *(u16 *)(p + 0x20) = s[0];
    *(u16 *)(p + 0x22) = s[1];
    *(u16 *)(p + 0x24) = s[2];
}
