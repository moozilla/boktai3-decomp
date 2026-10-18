#include "global.h"
#define TransitionPanel_InitOverlay sub_081631F8
struct TransitionPanelResource {
    u32 words[8];
};
struct TransitionPanelOverlay {
    u8 unknown0[0x3A];
    u16 frame;
    u8 unknown3C[12];
    void *tiles;
    u8 unknown4C[0x14];
};
struct TransitionPanel {
    u8 unknown0[0x298];
    struct TransitionPanelResource resource;
    struct TransitionPanelOverlay overlay;
    u32 tileCopy[8];
    u8 unknown338[11], overlayState;
};
struct TransitionPanelPosition {
    s32 x : 16, y : 16;
    s32 z : 16, pad : 16;
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void sub_08219918(void *, void *, s32, s32, s32, s32, s32, void *);
void sub_081631F8(struct TransitionPanel *p)
{
    struct TransitionPanelResource *resource = ScriptProc_Start(0xCB05, 0x530D);
    if (resource) {
        struct TransitionPanelPosition position;
        p->resource = *resource;
        ResourceHeader_Relocate(&p->resource, resource);
        position.x = -8;
        position.y = -8;
        position.z = 0;
        sub_08219918(&p->overlay, &p->resource, 0x4B, 0x1091, 0, 0, 60, &position);
        CpuSet(p->overlay.tiles, p->tileCopy, 0x04000008);
        p->overlay.frame = 0x3AA;
        p->overlay.tiles = p->tileCopy;
        p->overlayState = 0;
    }
}
