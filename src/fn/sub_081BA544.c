#include "global.h"
extern u8 *gUnk_0200025C;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 sub_0821ABA8(u32, u32);
void sub_081BA12C(u8 *, u32, s32);
void sub_081BA544(void)
{
    u8 *p = gUnk_0200025C;
    if (p) {
        if (Script_SeekToKeyword(0x66)) {
            s32 v = Script_GetValue();
            sub_081BA12C(p, sub_0821ABA8(0x74, 7), v);
        }
    }
}
