#include "global.h"
#include "link/serial/serial.h"

bool32 sub_08222C44(void)
{
    if (gSerialMultiplayer1Word != 0xffff)
        return TRUE;
    return FALSE;
}
