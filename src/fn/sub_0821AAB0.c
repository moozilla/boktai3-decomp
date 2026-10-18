#include "global.h"
u32 Script_ReadValueSafe(u8 *);
u32 Script_ReadString(u8 *p)
{
    return Script_ReadValueSafe(p);
}
