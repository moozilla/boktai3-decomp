#define Presentation_DrawEntryCount sub_08174B8C
#define Presentation_DrawDecimal sub_08165488
#include "global.h"
#include "presentation_entry_list.h"
void sub_08165488(s32, s32, s32, s32, s32);
void sub_08166534(void *, s32, s32);
void sub_08174B8C(struct PresentationEntryList *panel)
{
    sub_08165488(panel->entryCount, 100, 14, 12, 0);
    sub_08166534(panel, 5, 13);
}
