#include "global.h"
struct S { u32 a; };
extern struct S *gUnk_02000208;
struct S *sub_0811DEC8(void)
{
    if (!gUnk_02000208) return 0;
    return &gUnk_02000208->a;
}
