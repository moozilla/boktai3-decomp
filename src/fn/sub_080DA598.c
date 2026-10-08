#include "global.h"

void sub_0807E7B0(void *, s32);
void sub_08219DD8(void *, s32);

struct S { u8 filler[0x5b8]; u8 a[8]; u8 filler2[2]; u16 h5c2; u16 h5c4; };
struct S2 { u8 filler[0x5c0]; u16 h5c0; u16 h5c2; };

void sub_080DA598(struct S2 *s)
{
    sub_0807E7B0(s, 4);
    sub_08219DD8((u8 *)s + 0x5b8, 8);
    s->h5c2 = 0;
    s->h5c0 = 0;
}
