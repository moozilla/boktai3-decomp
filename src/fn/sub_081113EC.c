#include "global.h"
#include "ram.h"
struct S {
    u8 f[0x1ae0];
    u32 flags;
};
void sub_082194D4(void);
u32 PaletteFade_OnTaskDestroy(struct S *s)
{
    if (s->flags & 1)
        sub_082194D4();
    return gPaletteFade = 0;
}
