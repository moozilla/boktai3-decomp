#include "global.h"

u32 Serial_GetRole(void)
{
    return *(u32 *)0x0300531C;
}
