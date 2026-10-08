#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08611E04[];
s32 sub_0819CFEC(void *);
void sub_0824923C(void *, u32);
void sub_0819E664(u8 *p) {
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (sub_0819CFEC(p) == 0) {
            sub_0824923C(p, gUnk_08611E04[p[0xad]]);
        }
    }
}
