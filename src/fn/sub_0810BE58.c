#include "global.h"
extern u8 *gUnk_020004B4;
void sub_0821AAD8(void *);
void sub_0821B4C8(void *, s32, s32);
static inline u8 *getBlock(void)
{
    u8 *p = gUnk_020004B4;
    if (p) {
        u8 state = p[0x53];
        p += 0xE0;
        if (state == 1)
            goto done;
    }
    p = 0;
done:
    return p;
}
/* Registry routine 1022: stores byte +0x15D of the object at 0x020004B4 in
   the result variable while its +0x53 is 1, else 0 (meaning unknown). */
void Registry1022_GetValueFromScript(void)
{
    u32 result[2];
    u8 *block;
    s32 value;
    sub_0821AAD8(result);
    block = getBlock();
    value = 0;
    if (block)
        value = block[0x7D];
    sub_0821B4C8(result, 0, value);
}
