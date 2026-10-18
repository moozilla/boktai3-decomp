#include "global.h"

void System_ShutdownAndSoftReset(void);

void Engine_SoftResetFromScript(void)
{
    System_ShutdownAndSoftReset();
}
