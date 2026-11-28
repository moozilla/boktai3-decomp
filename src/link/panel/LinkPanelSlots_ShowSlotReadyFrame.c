#include "global.h"
#include "link/panel/link_panel.h"

void LinkPanelSlots_ShowSlotReadyFrame(struct LinkPanel *panel, s32 i)
{
    panel->animations[i].flags &= ~1;
    panel->selected[i] = 0x2c;
}
