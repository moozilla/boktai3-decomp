#include "global.h"
struct S {
    u8 f[0x1828];
    s32 cnt;
    u32 pad;
    u32 b;
    u32 c;
    u32 e;
    s32 d;
    u32 flags;
};
void PaletteFade_AdvanceColors(void *);
void sub_08111578(void *);
void PaletteFade_UpdateTransition(struct S *s)
{
    if (s->flags & 1) {
        if (s->cnt <= 0) {
            s->flags &= ~1;
            if (s->d > 0)
                sub_08111578(s);
        } else {
            PaletteFade_AdvanceColors(s);
            s->cnt--;
        }
    }
}
