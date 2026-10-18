#define Presentation_UpdateListCursor sub_08174E38
#include "global.h"
#include "presentation_entry_list.h"
void sub_08174E38(struct PresentationEntryList *panel)
{
    panel->cursorX = 0;
    panel->cursorY = panel->rowIndex * 8;
}
