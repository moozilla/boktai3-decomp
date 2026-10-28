#include "global.h"
#include "engine.h"
#include "ram.h"
/* Registry3C9F_Destroy: Hides menu display objects and clears backgrounds before killing the singleton task */
#define Registry3C9F_Destroy sub_0810E928

void Registry3C9F_HideDisplayObjects(void);
void ScrollingDialogue_ClearScript(void);

void sub_0810E928(void)
{
    if (gConfirmPanel) {
        Registry3C9F_HideDisplayObjects();
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        ScrollingDialogue_ClearScript();
        Task_Kill(gConfirmPanel);
    }
}
