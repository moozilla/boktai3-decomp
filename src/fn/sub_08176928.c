#include "global.h"
#define Hud_SetPhase Hud_SetUpdateCallback
#define Hud_SetBlend Hud_InitSelectionBounds
struct HudManager {
    u8 pad0[0xA4C];
    u16 timer;
    u8 padA4E[0xA];
    u8 state;
};
void SceneHud_DrawSelectedClock(struct HudManager *);
void Hud_InitSelectionBounds(u32, u32);
void Hud_SetUpdateCallback(struct HudManager *, void (*)(void));
void SceneHud_UpdatePageInput(void);
void Hud_UpdateBlendRamp(struct HudManager *p)
{
    SceneHud_DrawSelectedClock(p);
    Hud_InitSelectionBounds(p->timer * 4, 0x1084);
    if (++p->timer > 16) {
        p->state = 0;
        Hud_SetUpdateCallback(p, SceneHud_UpdatePageInput);
    }
}
