#include "global.h"

struct In {
    u8 f[0xf];
    u8 a;
};
struct S {
    u8 f[0x18];
    struct In in;
};
void MessageWindow_ClearTextCompletion(struct S *p)
{
    struct In *q = &p->in;
    q->a = 0;
}
