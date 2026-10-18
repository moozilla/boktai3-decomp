#include "global.h"
#include "engine.h"
/* ScrollingDialogue_SetNumberArgumentFromScript: Reads an argument index and value from the script and sets the number argument */
#define ScrollingDialogue_SetNumberArgumentFromScript sub_080334A0

s32 ScrollingDialogue_SetNumberArgument(s32, s32);

s32 sub_080334A0(void)
{
    s32 a = Script_GetValue();
    return ScrollingDialogue_SetNumberArgument(a, Script_GetValue());
}
