#include "global.h"
u8 *Script_GetPc(void);
u32 Script_ReadValueSafe(u8 *);
u32 Script_GetValueSafe(void)
{
    return Script_ReadValueSafe(Script_GetPc());
}
