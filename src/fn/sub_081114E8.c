#include "global.h"
struct S { u8 f[0x1828]; s32 cnt; u32 pad; u32 b; u32 c; u32 e; s32 d; u32 flags; };
void sub_08111410(void *);
void sub_08111578(void *);
void sub_081114E8(struct S *s)
{
    if (s->flags & 1) {
        if (s->cnt <= 0) {
            s->flags &= ~1;
            if (s->d > 0)
                sub_08111578(s);
        } else {
            sub_08111410(s);
            s->cnt--;
        }
    }
}
