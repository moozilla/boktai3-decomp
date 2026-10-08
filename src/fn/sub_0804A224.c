#include "global.h"
extern u8 *gUnk_02000488;
void sub_0821AAD8(void *);
void sub_0821B4C8(void *, u32, s32);
void sub_0804A224(void)
{
    u8 *p = gUnk_02000488;
    u32 buf[2];
    if (p) {
        sub_0821AAD8(buf);
        sub_0821B4C8(buf, 0, *(s16 *)(p + 0x50));
        sub_0821AAD8(buf);
        sub_0821B4C8(buf, 0, *(s16 *)(p + 0x52));
        sub_0821AAD8(buf);
        sub_0821B4C8(buf, 0, *(s16 *)(p + 0x54));
        sub_0821AAD8(buf);
        sub_0821B4C8(buf, 0, *(u8 *)(p + 0x260));
    }
}
