#include "global.h"
struct S {
    u8 f[0x1ae0];
    u32 flags;
};
extern u32 gUnk_020001D8;
void sub_082194D4(void);
u32 PaletteFade_OnTaskDestroy(struct S *s)
{
    if (s->flags & 1)
        sub_082194D4();
    return gUnk_020001D8 = 0;
}
