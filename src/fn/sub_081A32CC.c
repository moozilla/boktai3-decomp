#include "global.h"

extern u8 *gUnk_020005AC;

u16 Hresvelgr_GetAnimationIdentifier(void)
{
    return *(u16 *)(gUnk_020005AC + 0x1F8);
}
