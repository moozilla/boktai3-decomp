#include "global.h"

struct S { u8 pad[0x90]; u8 a[0xA8]; u16 st; u16 pad2[2]; u16 d; };
void sub_08215284(void *, u32);

void sub_080640AC(struct S *s)
{
    if (s->d <= 5)
        sub_08215284(s->a, 0x192);
    else
        sub_08215284(s->a, 0x193);
    s->st = 2;
    s->d++;
}
