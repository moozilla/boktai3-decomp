#include "global.h"
struct S { u8 f[0x2e8]; void *arr[4]; void *a2f8; u32 pd; u32 st; };
u8 sub_081C55B8(void *);
void sub_0821AD08(void *, u32);
void sub_0821A0C0(void *);
void sub_081C583C(struct S *p)
{
    if (sub_081C55B8(p) != 0) {
        u32 s = p->st;
        if (s == 5) {
            sub_0821AD08(p->a2f8, 0);
            sub_0821A0C0(p);
        } else if (s <= 3) {
            void *q = p->arr[s];
            if (q != 0) {
                sub_0821AD08(q, 0);
                sub_0821A0C0(p);
            }
        }
    }
}
