#include "global.h"

void ScriptUtility_SetControlFlag(u8);

void Actor_SetInteractionControlFlag(u8 *p)
{
    ScriptUtility_SetControlFlag(*(u8 *)(p + 0xF6A));
}
