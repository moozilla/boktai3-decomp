#include "global.h"
#include "presentation_entry_list.h"
void Hud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void MenuDisplay_DrawStatComparison(void *, s32, s32);
void Presentation_DrawEntryCount(struct PresentationEntryList *panel)
{
    Hud_DrawDecimalDigits(panel->entryCount, 100, 14, 12, 0);
    MenuDisplay_DrawStatComparison(panel, 5, 13);
}
