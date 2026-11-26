#include "global.h"

u32 Script_RunBlock(u32, void *);

u32 Script_ExecuteBlockWrapper(u32 a, void *b)
{
    return Script_RunBlock(a, b);
}
