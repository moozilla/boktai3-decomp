#include "global.h"
extern u8 *gGameState;
u32 Collection_IsEntryUnlocked(u32 i)
{
    return *(u32 *)(gGameState + 0x728) & (1 << i);
}
