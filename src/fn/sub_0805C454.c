#include "global.h"

struct P { u8 f[0x1a]; u16 v; };
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);

void sub_0805C454(struct P *p)
{
    s32 r = Script_SeekToKeyword(0x6d);
    if (r != 0)
        r = Script_GetValue();
    p->v = r;
}
