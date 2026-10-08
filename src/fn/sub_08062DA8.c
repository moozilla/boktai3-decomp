#include "global.h"

s32 Script_SeekToKeyword(s32);
u32 sub_08227E90(void);
void sub_08033568(void);
void sub_08030F20(s32, s32, s32, s32);
void sub_08030B78(s32);
void sub_080311B0(s32);

void sub_08062DA8(u8 *p)
{
    if (Script_SeekToKeyword(0x73))
        *(u32 *)(p + 0x1A88) = sub_08227E90();
    sub_08033568();
    sub_08030F20(0, 0x10, 0x1e, 2);
    sub_08030B78(1);
    sub_080311B0(0x81B);
}
