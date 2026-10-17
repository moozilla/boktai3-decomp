#include "global.h"

#define Pool1B8_IsReadyFromScript sub_081082A8
#include "engine.h"
u8 *Pool1B8_FindByName(s32);
/* Script: 1 when the pool-1B8 slot named -i has both bits 0x180 of +0xC0
   set, else 0. */
u32 Pool1B8_IsReadyFromScript(void)
{
    s32 name = Script_GetKeywordValue('i', -1);
    u8 *tile;
    if (name < 0 || (tile = Pool1B8_FindByName(name)) == 0 ||
        (*(u16 *)(tile + 0xC0) & 0x180) != 0x180)
        return 0;
    return 1;
}
