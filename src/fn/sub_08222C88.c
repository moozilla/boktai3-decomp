#include "global.h"

void Serial_DisableGlobalInterrupts(void)
{
    *(vu16 *)0x04000208 = 0;
}
