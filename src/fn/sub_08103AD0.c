// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* NamedObject_ResetManager: Clears the singleton named object manager pointer */
#define NamedObject_ResetManager sub_08103AD0

void sub_08103AD0(void)
{
    gNamedActorManager = 0;
}
