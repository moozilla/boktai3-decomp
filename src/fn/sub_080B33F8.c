#include "global.h"
void Vorn_ResetRecoveryMode(u8 *p)
{
    p[0x6c0] = 0;
}
