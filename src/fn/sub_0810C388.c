#include "global.h"

void ScrollingDialogue_ClearScript(void);
s32 ScrollingDialogue_RequestShow(void);

void RegistryF246_InitMessageDisplay(void)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestShow();
}
