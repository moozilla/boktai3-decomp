#include "global.h"

void Hresvelgr_SetLinkedActor(u8 *p, u32 v)
{
    *(u32 *)(p + 0x420) = v;
}
