#include "global.h"
#include "runtime.h"
u8 Save_ReadContinuation(u8);

u8 sub_0822BE60(void)
{
    return Save_ReadContinuation(gSessionState[8]);
}
