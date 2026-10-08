#include "global.h"

struct S { u8 pad[0x5c]; u32 f5c; u32 f60; u8 pad2[4]; s32 f68; };
void sub_081C439C(struct S *);
void sub_0821AD08(u32, u32);
void sub_0821A0C0(u32);
void sub_081C44E0(struct S *);

void sub_081C4668(struct S *p)
{
    sub_081C439C(p);
    p->f68++;
    if (p->f68 > 0x3f) {
        if (p->f5c) {
            sub_0821AD08(p->f5c, 0);
            sub_0821A0C0(p->f60);
            sub_0821A0C0((u32)p);
        } else {
            sub_081C44E0(p);
        }
    }
}
