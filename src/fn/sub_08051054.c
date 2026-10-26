#include "orbit_background.h"
struct OrbitBackground *OrbitBackground_Create(u32, s32, u32);
struct OrbitBackground *OrbitBackground_CreateFromScript(u32 id)
{
    s32 k2;
    s32 player;
    u32 enabled;
    if (gEngineFlags & 0x800) {
        if (!gUnk_020004B4) {
            k2 = 1;
            player = -k2;
        } else {
            player = gUnk_020004B4->selected;
        }
    } else if (Script_SeekToKeyword('i')) {
        player = Script_GetValue();
    } else {
        player = 0;
    }
    if (Script_SeekToKeyword('m')) {
        enabled = Script_GetValue();
    } else {
        enabled = 1;
    }
    return OrbitBackground_Create(id, player, enabled);
}
