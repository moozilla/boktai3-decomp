#include "global.h"

void VanargandHead_DispatchPendingCommands(u8 *);

void VanargandHead_ResetDisplayState(u8 *p)
{
    *(u8 *)(p + 0xAA) = 0;
    VanargandHead_DispatchPendingCommands(p);
}
