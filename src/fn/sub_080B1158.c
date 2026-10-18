#include "global.h"
void Vorn_InitActionCooldown(u8 *p)
{
    *(u16 *)(p + 0x692) = 0x78;
    *(u16 *)(p + 0x690) = 0x78;
}
