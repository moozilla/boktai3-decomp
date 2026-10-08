#include "global.h"
struct S { u8 pad[9]; u8 f9; u8 fa; u8 fb; };
extern struct S *gUnk_030053F8;
void sub_082254A8(void)
{
    if (gUnk_030053F8) {
        gUnk_030053F8->f9 = 0;
        gUnk_030053F8->fb = 0;
        gUnk_030053F8->fa = 0;
    }
}
