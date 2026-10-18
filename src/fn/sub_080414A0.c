#include "global.h"

void ScrollingDialogue_HideImmediately(void);
void ModifierMenu_DestroyTextWindows(u8 *);

void ModifierMenu_ReleaseTextResources(u8 *p)
{
    u8 *f = p + 0x116;
    if (*f == 0) {
        ScrollingDialogue_HideImmediately();
        ModifierMenu_DestroyTextWindows(p);
        *f = 1;
    }
}
