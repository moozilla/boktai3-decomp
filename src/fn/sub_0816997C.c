
#include "global.h"
#include "render.h"
struct SelectionPanel {
    u8 pad[0xA5E];
    u8 state;
};
void Hud_DrawHealthMeter(struct SelectionPanel *, u32, u32, u32, u32);
void SceneHud_TickWidgetSlides(struct SelectionPanel *);
void SelectionPanel_RefreshPrimaryIfReady(struct SelectionPanel *p)
{
    if (!(gEnginePauseFlags & 1) && p->state <= 2) {
        Hud_DrawHealthMeter(p, 0, 4, 1, 1);
        SceneHud_TickWidgetSlides(p);
    }
}
