#include "global.h"
s32 sub_08109464(void *);
void sub_080335B4(void);
void sub_0821A0C0(void *);
void sub_0824923C(void *, u32);
struct S { u8 filler[0x14D8]; u32 f; };
s32 sub_0810F604(struct S *s)
{
    if (sub_08109464(s) == 0) {
        sub_080335B4();
        sub_0821A0C0(s);
        return -1;
    }
    if (s->f != 0)
        sub_0824923C(s, s->f);
    return 1;
}
