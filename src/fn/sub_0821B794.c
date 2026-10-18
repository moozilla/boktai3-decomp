#include "global.h"
u32 Script_SelectConditionalBlock(void);
u32 Script_ExecuteBlock(u32, u32, u32);
u32 Script_ExecuteIf(void)
{
    u32 r = Script_SelectConditionalBlock();
    if (r != 0)
        return Script_ExecuteBlock(r, 0, 0);
    return 0;
}
