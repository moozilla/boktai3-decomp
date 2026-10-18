#include "global.h"
#define FieldHud_RefreshGaugeIndex sub_081793D0
#include "field_hud_widgets.h"
void PaletteFade_SetScrollPosition(s32, s32);
void sub_081793D0(struct FieldHudWidgets *p)
{
    s32 index = p->fill * 2 + 1;
    if (p->displayedIndex != index)
        PaletteFade_SetScrollPosition(index, 0);
}
