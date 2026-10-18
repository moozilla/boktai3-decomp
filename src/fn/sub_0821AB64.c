#include "global.h"
#include "engine.h"
/* Script_GetVector: Decodes three operands at the current script cursor into a signed 16-bit vector */
#define Script_GetVector sub_0821AB64

s32 Script_ReadVector16(void *, void *);

s32 sub_0821AB64(void *p)
{
    return Script_ReadVector16(Script_GetPc(), p);
}
