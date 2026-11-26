#include "global.h"

void Serial_StartTransfer(void);

void SerialLink_StartTransferIfIdle(void)
{
    s32 v = *(s32 *)0x03005324;
    if (v >= 0 || v <= -3) {
        if (v == 0)
            Serial_StartTransfer();
    }
}
