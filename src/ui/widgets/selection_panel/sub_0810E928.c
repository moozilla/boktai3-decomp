#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
/* RecordPanel_Destroy: Hides menu display objects and clears backgrounds before killing the singleton task */

void SelectionPanel_HideDisplayObjects(void *);
void ScrollingDialogue_ClearScript(void);

void RecordPanel_Destroy(void)
{
    if (gConfirmPanel) {
        SelectionPanel_HideDisplayObjects(gConfirmPanel);
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gConfirmPanel);
    }
}
