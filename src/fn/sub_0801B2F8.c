#include "global.h"

void SelectionMenu_RemoveIconGrid(u32);
void SelectionMenu_RemoveCursors(u32);
void SelectionMenu_RemoveMarkerAnim(u32);
void SelectionMenu_RemoveHighlight(u32);
void SelectionMenu_RemovePanel(u32);
void ScrollingDialogue_RequestHide(void);
u32 SelectionMenu_Destroy(u32 p)
{
    SelectionMenu_RemoveIconGrid(p);
    SelectionMenu_RemoveCursors(p);
    SelectionMenu_RemoveMarkerAnim(p);
    SelectionMenu_RemoveHighlight(p);
    SelectionMenu_RemovePanel(p);
    ScrollingDialogue_RequestHide();
    return 0;
}
