#include "global.h"
struct S {
    u8 p[8];
    u8 b;
};
extern struct S *gUnk_030053F8;
u8 Save_WriteHeader(void);
u8 Save_ToggleSelectedSlot(void)
{
    struct S *s = gUnk_030053F8;
    s->b = 1 - s->b;
    return Save_WriteHeader();
}
