#include "global.h"
#include "ui/town/shop_panel.h"
#include "ui/town/storage_screen.h"
void MenuPanel_ShowDescription(struct StorageScreen *screen, s32 message)
{
    ScrollingDialogue_ClearScript();
    screen->indicators[4].flags |= 1;
    MessageWindow_SetMessageTable(screen->text);
    MessageWindow_SetBounds(0, 16, 30, 2);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_ShowMessage(message);
}
