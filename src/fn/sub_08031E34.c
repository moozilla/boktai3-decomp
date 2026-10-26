#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x19c];
    u32 a;
};
u32 MessageWindow_GetTextCompletionState(void)
{
    struct S *p = gMessageWindow;
    if (!p)
        return 0;
    return p->a;
}
