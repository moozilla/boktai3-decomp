#include "global.h"
void MenuCursor_OnHidden(void);
void SelectionCursor_SetState(void *, void (*)(void));
void MenuCursor_Hide(void *p)
{
    if (p != 0)
        SelectionCursor_SetState(p, MenuCursor_OnHidden);
}
