#include "global.h"
#include "link/panel/link_panel.h"
void TextWindow_Clear(u32);

void sub_0810BF60(struct LinkPanel *panel)
{
    TextWindow_Clear(panel->mainWindow);
    TextWindow_Clear(panel->firstWindow);
    TextWindow_Clear(panel->secondWindow);
}
