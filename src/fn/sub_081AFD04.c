#include "global.h"

void Vanargand_ClearPartContact(u8 *p)
{
    *(u32 *)(p + 0x160) = 0;
}
