#include "global.h"

extern u8 *gUnk_02000710;

void sub_0811D384(u8 *);
void sub_0811CC24(u8 *, u8 *);
void sub_0811C818(u8 *, s32);

void sub_08041908(u8 *p) {
    u8 *a = p + 0x83C;
    u8 *b = p + 0xE34;
    sub_0811D384(p + 0x884);
    sub_0811CC24(a, b);
    sub_0811C818(b, *(s16 *)(gUnk_02000710 + 0x81A));
}
