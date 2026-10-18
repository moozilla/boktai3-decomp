#include "global.h"
#include "presentation_entry_list.h"
void Presentation_UpdateListCursor(struct PresentationEntryList *panel)
{
    panel->cursorX = 0;
    panel->cursorY = panel->rowIndex * 8;
}
