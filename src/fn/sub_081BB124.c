#include "global.h"
void MenuCursor_SetPositionFromSelection(u8 *);

void sub_081BB124(u8 *menu, u32 selection)
{
    if (menu != NULL) {
        *(u16 *)(menu + 0xa0) = selection;
        MenuCursor_SetPositionFromSelection(menu);
    }
}
