#include "global.h"

void ScriptRecord_FindModifier(u16 *, s32);

void Modifier_LoadThirdSource(s16 *p)
{
    u16 buf[6];
    ScriptRecord_FindModifier(buf, p[0x10]);
    p[7] = buf[1];
    p[8] = buf[2];
    p[9] = buf[3];
}
