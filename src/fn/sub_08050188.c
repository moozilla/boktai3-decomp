#include "global.h"

struct S { u8 filler[0x32]; s16 v; };
extern struct S *gUnk_02000580;
struct P { u8 filler[0x20]; u16 w; };

s32 sub_08050188(struct P *p)
{
    if (gUnk_02000580->v == (s32)(p->w << 8))
        return 1;
    return 0;
}
