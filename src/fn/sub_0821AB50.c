#include "global.h"
#include "engine.h"

void *Script_GetPc(void);
s32 sub_0821AA1C(void *, void *);

s32 sub_0821AB50(void *p)
{
    return sub_0821AA1C(Script_GetPc(), p);
}
