#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08604EF0[];
s32 sub_08023D78(void *);
void sub_0824923C(void *, u32);
void sub_08024C48(u8 *p) {
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (sub_08023D78(p) == 0) {
            sub_0824923C(p, gUnk_08604EF0[p[0xad]]);
        }
    }
}
