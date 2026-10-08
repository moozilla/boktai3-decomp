#include "global.h"
struct S { u8 f[0x102E]; u16 h2; u16 h3; };
void sub_0822B2F8(s32);
void sub_0810FEDC(struct S *, s32, s32);
void sub_08110064(struct S *s)
{
    u16 *p = &s->h2;
    if (*p != 0) {
        *p -= 1;
        if (*p == 0) {
            sub_0810FEDC(s, 0, s->h3);
            sub_0822B2F8(0x167);
        }
    }
}
