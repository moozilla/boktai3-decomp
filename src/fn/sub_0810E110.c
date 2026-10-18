#include "global.h"

void ScrollingDialogue_ClearScript(void);
s32 ScrollingDialogue_RequestShow(void);

void Registry3C9F_InitTextState(u8 *p)
{
    *(u8 *)(p + 0xD05) = 0xFF;
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestShow();
}
