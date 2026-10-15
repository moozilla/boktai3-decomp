#include "global.h"

void ScriptUtility_SetControlFlag(u8);

void sub_081E4174(u8 *p)
{
    ScriptUtility_SetControlFlag(*(u8 *)(p + 0xF6A));
}
