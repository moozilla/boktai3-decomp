#include "global.h"
extern u8 *gUnk_02000090;

bool32 sub_08017428(void)
{
    if (gUnk_02000090 != 0)
        return TRUE;
    return FALSE;
}
