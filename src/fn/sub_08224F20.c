#include "global.h"

u32 Script_Call(void);

u32 Script_CallProcedureWrapper(void)
{
    return Script_Call();
}
