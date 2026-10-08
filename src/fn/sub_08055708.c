#include "global.h"
u8 *sub_08055678(void);
s32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void sub_08055708(void)
{
    u8 *p = sub_08055678();
    if (p) {
        if (Script_SeekToKeyword(0x66)) {
            if (Script_GetValue() != 0)
                *(u32 *)(p + 0x40) |= 4;
            else
                *(u32 *)(p + 0x40) &= -5;
        }
    }
}
