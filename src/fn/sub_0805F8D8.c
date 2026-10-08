#include "global.h"

struct S { u8 f[8]; s16 v; };
struct S *sub_0822E288(void);

s32 sub_0805F8D8(void)
{
    return sub_0822E288()->v;
}
