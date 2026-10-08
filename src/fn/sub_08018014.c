#include "global.h"

extern u8 *gUnk_02000090;
void sub_08017700(void);

u32 sub_08018014(void) {
    sub_08017700();
    gUnk_02000090 = 0;
    return 0;
}
