#include "global.h"
#include "engine.h"
#include "ram.h"
void MenuCursor_EnableInput(void *);
void MenuCursor_Hide(void *);
/* Script: for the registry 2388 task (gSelectionPanel) with a child at +0x558:
   -v nonzero clears +0x52A and calls MenuCursor_EnableInput, else sets it and calls
   MenuCursor_Hide (tentative). */
void Registry2388_SetVisibleFromScript(void)
{
    u8 *p = gSelectionPanel;
    void **child;
    if (!p)
        return;
    child = (void **)(p + 0x558);
    if (!*child)
        return;
    if (Script_GetKeywordValue('v', 0)) {
        *(u16 *)(p + 0x52A) = 0;
        MenuCursor_EnableInput(*child);
    } else {
        *(u16 *)(p + 0x52A) = 1;
        MenuCursor_Hide(*child);
    }
}
