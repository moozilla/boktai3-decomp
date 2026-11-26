#include "engine/shared/global.h"
#include "link/serial/serial.h"

bool32 sub_08222C24(void)
{
    if (gSerialMultiplayer0Word != 0xffff)
        return TRUE;
    return FALSE;
}
