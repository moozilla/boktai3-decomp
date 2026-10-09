#include "global.h"
s32 sub_081669E8(void *, u32, u32);
u32 sub_08166A94(void *ctx, u32 a, u32 b, u32 c) {
    if (a == b) return 0;
    if (sub_081669E8(ctx, a, c) == 0) return 1;
    if (sub_081669E8(ctx, b, c) == 0) return 1;
    return 0;
}
