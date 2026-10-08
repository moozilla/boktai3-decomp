#include "global.h"

extern const u8 gUnk_086049AC[];
u32 sub_08004E8C(u32 a, u32 b);

u32 sub_08005098(u32 a, u32 i) {
    return sub_08004E8C(a, gUnk_086049AC[i]);
}
