#include "global.h"
#include "engine.h"

s32 sub_08033478(s32, s32);

s32 sub_080334A0(void)
{
    s32 a = Script_GetValue();
    return sub_08033478(a, Script_GetValue());
}
