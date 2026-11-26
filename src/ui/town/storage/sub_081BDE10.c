#include "global.h"
struct H {
    u8 f[8];
    u32 v;
};
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void Storage_HideItemDetails(u8 *p)
{
    ((struct H *)p)[354].v |= 1;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
}
