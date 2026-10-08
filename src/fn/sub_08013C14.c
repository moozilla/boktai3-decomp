#include "global.h"

extern u32 gUnk_02000470;
u32 sub_08013CF4(void);

u32 sub_08013C14(void) {
    u32 r = gUnk_02000470;
    if (r == 0) {
        return sub_08013CF4();
    }
    return r;
}
