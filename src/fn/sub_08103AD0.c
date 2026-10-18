// COMPILER: old_agbcc
#include "global.h"
/* NamedObject_ResetManager: Clears the singleton named object manager pointer */
#define NamedObject_ResetManager sub_08103AD0

extern u32 gUnk_020001AC;

void sub_08103AD0(void)
{
    gUnk_020001AC = 0;
}
