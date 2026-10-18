#include "global.h"
u8 *Script_GetPc(void);
u32 Script_SeekToKeyword(u8);
u32 Script_ReadValue(u8 *);
u32 Script_GetKeywordValue(u8 k, u32 d)
{
    if (Script_SeekToKeyword(k) != 0)
        return Script_ReadValue(Script_GetPc());
    return d;
}
