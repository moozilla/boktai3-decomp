#include "global.h"

u32 Script_RunBlock(void);

u32 Script_ExecuteBlockWrapper(void)
{
    return Script_RunBlock();
}
