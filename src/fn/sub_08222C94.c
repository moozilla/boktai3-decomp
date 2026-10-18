#include "global.h"

void Serial_EnableGlobalInterrupts(void)
{
    *(vu16 *)0x04000208 = 1;
}
