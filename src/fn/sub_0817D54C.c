#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08611CB8[];
s32 sub_0817B694(void *);
void sub_0824923C(void *, u32);
void sub_0817D54C(u8 *p) {
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (sub_0817B694(p) == 0) {
            sub_0824923C(p, gUnk_08611CB8[p[0xad]]);
        }
    }
}
