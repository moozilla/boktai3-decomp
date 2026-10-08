#include "global.h"

extern u8 *gUnk_02000028;
void sub_08005F78(void);

u32 sub_08006120(void) {
    sub_08005F78();
    gUnk_02000028 = 0;
    return 0;
}
