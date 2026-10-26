#include "global.h"
#include "ram.h"

u16 Hresvelgr_GetAnimationIdentifier(void)
{
    return *(u16 *)(gHresvelgr + 0x1F8);
}
