#include "global.h"

void Serial_ResetMultiplayer(void);

void Serial_InitMultiplayer(void)
{
    *(s32 *)0x03005324 = -1;
    Serial_ResetMultiplayer();
}
