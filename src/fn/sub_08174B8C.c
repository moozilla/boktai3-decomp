#include "global.h"
#include "presentation_entry_list.h"
void SceneHud_DrawDecimalDigits(s32, s32, s32, s32, s32);
void sub_08166534(void *, s32, s32);
void Presentation_DrawEntryCount(struct PresentationEntryList *panel)
{
    SceneHud_DrawDecimalDigits(panel->entryCount, 100, 14, 12, 0);
    sub_08166534(panel, 5, 13);
}
