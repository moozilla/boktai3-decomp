#include "global.h"
#include "serial.h"

bool32 sub_08222C44(void)
{
    if (gSerialMultiplayer1Word != 0xffff)
        return TRUE;
    return FALSE;
}
