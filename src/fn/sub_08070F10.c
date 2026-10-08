#include "global.h"
s32 Script_GetValue(void);
s32 sub_08070EBC(void);
u32 sub_08070F10(void)
{
    if (Script_GetValue() != 0 && sub_08070EBC() != 0)
        return 1;
    return 0;
}
