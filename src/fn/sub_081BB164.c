#include "global.h"
void MenuCursor_OnSelectionFlag(void);
void SelectionCursor_SetState(void *, void (*)(void));
void MenuCursor_SetSelectionFlag(void *p)
{
    if (p != 0)
        SelectionCursor_SetState(p, MenuCursor_OnSelectionFlag);
}
