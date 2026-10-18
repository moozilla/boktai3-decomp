#include "global.h"
void MenuCursor_UpdateInput(void);
void SelectionCursor_SetState(void *, void (*)(void));
void MenuCursor_EnableInput(void *p)
{
    if (p != 0)
        SelectionCursor_SetState(p, MenuCursor_UpdateInput);
}
