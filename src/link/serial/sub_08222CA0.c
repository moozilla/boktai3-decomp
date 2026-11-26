#include "global.h"

void Serial_StartTransfer(void)
{
    *(vu16 *)0x04000128 |= 0x80;
}
