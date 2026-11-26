#include "global.h"

struct S {
    u8 f[0x150];
    u32 a;
};
void TextState_ClearSelectedChoices(struct S *p)
{
    p->a = 0;
}
