#include "global.h"
#include "field_hud_widgets.h"
void PaletteFade_SetScrollPosition(s32, s32);
void FieldHud_RefreshGaugeIndex(struct FieldHudWidgets *p)
{
    s32 index = p->fill * 2 + 1;
    if (p->displayedIndex != index)
        PaletteFade_SetScrollPosition(index, 0);
}
