#include "global.h"

void StoryCharacter_DispatchCommand(u32, u32);

s32 StoryCharacter_OnCommand(u32 a, u32 b)
{
    StoryCharacter_DispatchCommand(a, b);
    return 0;
}
