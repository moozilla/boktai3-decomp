#include "global.h"
extern const u32 gUnk_08603300[];

void Script_InitActorRegistry(void)
{
    vu16 *count = (vu16 *)0x030025F4;
    const u32 *entry;
    *count = 0;
    for (entry = gUnk_08603300; entry[1] != 0; entry += 2)
        (*count)++;
}
