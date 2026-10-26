#define WoodenCrate_TestFlags WoodenCrate_HasFlags
#include "global.h"
#include "engine.h"
#include "ram.h"
struct WoodenCrate {
    u8 unknown0[0x2C];
    u8 animation[4];
    u8 animationFlags;
    u8 unknown31[2];
    u8 animationLength;
    u8 unknown34[6];
    u16 animationFrame;
    u8 unknown3C[0x88];
    u16 state;
    u8 unknownC6[2];
    u16 flags;
    u8 animationRow;
    u8 unknownCB;
    u8 sequenceFlags;
};
struct WoodenCrateManager {
    u8 unknown0[0x18];
    void *animationData;
};
u8 WoodenCrate_HasFlags(struct WoodenCrate *, s32);
void WoodenCrate_ClearFlags(struct WoodenCrate *, u16);
void WoodenCrate_BeginToggleAnimation(struct WoodenCrate *crate)
{
    struct WoodenCrateManager *manager = gWoodenCrateManager;
    Sound_PlaySong(0x14C);
    if (crate->state == 2) {
        if (crate->animationFlags & 4)
            crate->animationFlags &= ~4;
        else
            crate->animationFlags |= 4;
        crate->animationFrame = crate->animationLength - crate->animationFrame;
    } else {
        if (WoodenCrate_HasFlags(crate, 1)) {
            Anim_SetSequence(crate->animation, manager->animationData, 1, crate->animationRow,
                             crate->sequenceFlags);
            WoodenCrate_ClearFlags(crate, 1);
        } else {
            Anim_SetSequence(crate->animation, manager->animationData, 1, crate->animationRow,
                             crate->sequenceFlags | 4);
            WoodenCrate_ClearFlags(crate, 2);
        }
        crate->state = 2;
    }
}
