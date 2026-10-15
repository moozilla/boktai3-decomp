#include "orbit_background.h"
// MATCHING_CONCESSION: register-pinning
struct OrbitBackground *OrbitBackground_Create(u32, s32, u32);
struct OrbitBackground *OrbitBackground_CreateFromScript(u32 id)
{
    register s32 player asm("r4");
    u32 enabled;
    if (gUnk_030053F4 & 0x800) {
        if (!gUnk_020004B4)
            player = -1;
        else
            player = gUnk_020004B4->selected;
    } else {
        if (Script_SeekToKeyword('i'))
            player = Script_GetValue();
        else
            player = 0;
    }
    if (Script_SeekToKeyword('m'))
        enabled = Script_GetValue();
    else
        enabled = 1;
    return OrbitBackground_Create(id, player, enabled);
}
