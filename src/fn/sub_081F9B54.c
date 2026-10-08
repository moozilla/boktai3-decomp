#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08613F48[];
s32 sub_081F79F8(void *);
void sub_0824923C(void *, u32);
void sub_081F9B54(u8 *p) {
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (sub_081F79F8(p) == 0) {
            sub_0824923C(p, gUnk_08613F48[p[0xad]]);
        }
    }
}
