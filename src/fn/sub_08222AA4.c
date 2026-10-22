#include "global.h"
extern s32 gUnk_0300531C;
void Serial_DisableTimer3Polling(void);
void Serial_DisableReceiveInterrupt(void);

s32 sub_08222AA4(void)
{
    if (gUnk_0300531C == 0)
        Serial_DisableTimer3Polling();
    else
        Serial_DisableReceiveInterrupt();
    return 0;
}
