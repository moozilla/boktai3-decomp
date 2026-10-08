#include "global.h"

extern u8 *gUnk_02000094;
void sub_08018438(u8 *p);

u32 sub_08018600(u8 *p) {
    gUnk_02000094 = p;
    *(u32 *)(p + 0x18) = 0;
    sub_08018438(p);
    return 0;
}
