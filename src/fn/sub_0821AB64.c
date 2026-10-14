#include "global.h"
#include "engine.h"

void *Script_GetPc(void);
s32 sub_0821AA50(void *, void *);

s32 sub_0821AB64(void *p)
{
    return sub_0821AA50(Script_GetPc(), p);
}
