#include "global.h"
s32 PauseMenu_GetPromptIconKeyframe(s32 a)
{
    if (a <= 1)
        return a + 0x7e;
    return a + 0x7f;
}
