#include "global.h"
#include "ui/town/storage_screen.h"
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void Storage_HideItemDetails(struct StorageScreen *screen)
{
    AnimSprite_Hide(&screen->indicators[4]);
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
}
