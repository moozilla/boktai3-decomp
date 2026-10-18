#include "global.h"
u8 *Script_GetPc(void);
u32 Script_ReadValue(u8 *);
u32 Script_GetValue(void)
{
    return Script_ReadValue(Script_GetPc());
}
