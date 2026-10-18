#include "global.h"
#include "engine.h"
/* Script_GetVector32: Reads a three component thirty two bit vector from the current script PC */
#define Script_GetVector32 sub_0821AB50

s32 Script_ReadVector32(void *, void *);

s32 sub_0821AB50(void *p)
{
    return Script_ReadVector32(Script_GetPc(), p);
}
