#include "global.h"

void ScriptRecord_FindExtendedModifier(u16 *, s32);

void Modifier_LoadFirstSource(s16 *p)
{
    u16 buf[8];
    ScriptRecord_FindExtendedModifier(buf, p[0xe]);
    p[0] = buf[3];
    p[1] = buf[2];
}
