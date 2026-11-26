#include "global.h"

u32 Script_Call(u32, void *);

u32 Script_CallProcedureWrapper(u32 id, void *b)
{
    return Script_Call(id, b);
}
