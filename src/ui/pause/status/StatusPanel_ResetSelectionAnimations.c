// Tentative StatusPanel family and semantic names; based on static consumers.
#include "global.h"
struct PanelSelectionAnim {
    u32 unknown[2];
    u32 flags;
    u8 rest[0x54];
};
struct StatusPanel {
    u8 unknown0[0xC4];
    u8 animationData[0x3D60 - 0xC4];
    struct PanelSelectionAnim selection[3];
    u8 unknown3E80[0x48E6 - 0x3E80];
    u8 selectionIndex;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void StatusPanel_ResetSelectionAnimations(struct StatusPanel *p)
{
    u32 mask = ~1;
    u32 *flags = &p->selection[0].flags;
    s32 i = 2;
    do {
        *flags &= mask;
        flags = (u32 *)((u8 *)flags + 0x60);
    } while (--i >= 0);
    Anim_LoadKeyframe(&p->selection[0], p->animationData, p->selectionIndex + 9, 1);
    Anim_LoadKeyframe(&p->selection[1], p->animationData, 10, 1);
    Anim_LoadKeyframe(&p->selection[2], p->animationData, 10, 1);
}
