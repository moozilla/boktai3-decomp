#include "global.h"

struct S { u8 pad[0x364]; u32 f364; u8 pad2[0x370-0x368]; s32 f370; };
void sub_0821AD08(u32, u32);
void sub_0821A0C0(struct S *);

void sub_081C616C(struct S *p)
{
    p->f370++;
    if (p->f370 > 0x20) {
        if (p->f364) {
            sub_0821AD08(p->f364, 0);
            sub_0821A0C0(p);
        }
    }
}
