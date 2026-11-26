#include "global.h"

void SingletonSlot7_DispatchPendingCommands(u8 *);

void SingletonSlot7_ResetDisplayState(u8 *p)
{
    *(u8 *)(p + 0xAA) = 0;
    SingletonSlot7_DispatchPendingCommands(p);
}
