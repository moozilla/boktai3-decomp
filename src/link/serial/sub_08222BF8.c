#include "engine/shared/global.h"
#include "link/serial/serial.h"

bool32 sub_08222BF8(void)
{
    if ((gSerialMultiplayer0Word | gSerialMultiplayer1Word) != 0xffff)
        return TRUE;
    return FALSE;
}
