#include "global.h"
#include "engine.h"

/* HUD interpretation and descriptive names are tentative. */
struct HudTransitionOffset {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct Hud {
    u8 pad0[0xA4];
    u8 primary[0xA0];
    u8 alternate[0x958];
    u8 transition[0x61];
    u8 transitionState;
};
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void Hud_InitializePanelTransition(struct Hud *hud)
{
    struct HudTransitionOffset offset;
    if (gEngineFlags & 0x1000) {
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        Widget_InitLinked(hud->transition, hud->alternate, 0x1c, 0x10031, 0, 0, 60, &offset);
    } else {
        offset.x = 8;
        offset.y = 32;
        offset.z = 0;
        Widget_InitLinked(hud->transition, hud->primary, 0x68, 0x10031, 0, 0, 60, &offset);
        hud->transitionState = 0;
    }
}
