#include "global.h"

s32 sub_081D4E4C(void);
s32 sub_081D4E5C(s32);
s32 sub_081D5274(s32);

u32 sub_081D52F0(s32 a) {
    s32 n = sub_081D4E4C();
    s32 base = sub_081D4E5C(a);
    s32 i;
    for (i = 0; i < n; i++) {
        if (sub_081D5274(base + i) == 0) {
            return 0;
        }
    }
    return 1;
}
