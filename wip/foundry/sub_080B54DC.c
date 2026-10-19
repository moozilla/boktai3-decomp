#include "global.h"

#define Vorn_CheckSomething sub_080B54DC

struct VornData {
    u8 unknown0[0x67C];
    s16 field67C;
    u8 unknown67E[0x6D4 - 0x67E];
    u16 field6D4;
};

u32 Vorn_CheckSomething(struct VornData *ctx)
{
    if (ctx->field6D4 & 1)
        return 1;
    if (ctx->field67C >= 0)
        return 1;
    return 0;
}
