#include "global.h"
#include "link/panel/link_panel.h"
void SlotMenu_InitializeEntryValues(struct LinkPanel *panel)
{
    panel->selected[0] = 0x2C;
    panel->selected[1] = 0x2C;
    panel->selected[2] = 0x2D;
    panel->selected[3] = 0x2E;
    panel->selected[4] = 0x27;
    panel->selected[5] = 0x28;
    panel->selected[6] = 0x7F;
}
