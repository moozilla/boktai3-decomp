#include "global.h"

u32 sub_08073C70(u32, u32);

u32 sub_08073CB0(u32 a, u32 b, s32 c) {
    return (u32)&((u32 *)sub_08073C70(a, b))[c + 2];
}
