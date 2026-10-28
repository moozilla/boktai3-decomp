#include "global.h"
#include "serial.h"

bool32 sub_08222C24(void)
{
    if (gSerialMultiplayer0Word != 0xffff)
        return TRUE;
    return FALSE;
}
