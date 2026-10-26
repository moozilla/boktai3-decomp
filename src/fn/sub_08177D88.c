#include "global.h"
#include "ram.h"

s32 sub_08177D88(void)
{
    s32 result;
    if (gFieldHud != NULL)
        result = *(u16 *)(gFieldHud + 0xa5c);
    else
        result = -1;
    return result;
}
