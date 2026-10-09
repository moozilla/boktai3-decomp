#include "global.h"
extern u8 *gUnk_02000710;
void *sub_0822D6C8(s32);
void sub_0816D4BC(void) {
    s16 i = *(s16 *)(gUnk_02000710 + 0x50);
    s16 v = *(s16 *)(gUnk_02000710 + 0x68 + i * 2);
    s16 *q = sub_0822D6C8(v);
    q[1] = 0;
}
