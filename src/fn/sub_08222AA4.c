#include "global.h"
#include "ram.h"
void Serial_DisableTimer3Polling(void);
void Serial_DisableReceiveInterrupt(void);

s32 sub_08222AA4(void)
{
    if (gSerialConnectionRole == 0)
        Serial_DisableTimer3Polling();
    else
        Serial_DisableReceiveInterrupt();
    return 0;
}
