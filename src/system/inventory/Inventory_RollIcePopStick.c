#include "global.h"
#include "engine/engine_globals.h"
#include "link/wireless/random.h"
s32 Mod(s32, s32);
u32 Inventory_RollIcePopStick(void)
{
    if (Mod(RANDOM_NEXT(), 100) < 2)
        return 0x1f;
    return 0x1e;
}
