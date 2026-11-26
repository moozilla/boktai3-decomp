#include "global.h"

void Serial_ResetTransferHardware(void);

u32 Serial_StopCommunication(void)
{
    if (*(s32 *)0x0300531C == 0)
        Serial_ResetTransferHardware();
    else
        Serial_ResetTransferHardware();
    if (*(s32 *)0x03005314 == 1)
        *(s32 *)0x03005314 = 0;
    return 0;
}
