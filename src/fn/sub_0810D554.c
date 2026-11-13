#include "global.h"
#include "engine.h"
#include "ram.h"

void LinkRecordPanel_HideDisplay(void);
void ScrollingDialogue_ClearScript(void);

void sub_0810D554(void)
{
    if (gPanelSequence) {
        LinkRecordPanel_HideDisplay();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gPanelSequence);
    }
}
