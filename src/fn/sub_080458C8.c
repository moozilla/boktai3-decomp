#include "global.h"
void sub_08219DD8(u32, u32);
void sub_08045934(u32, u32, u32, u32, u32);
void sub_08045950(u32, u32, u32, u32, u32, u32);
void sub_08045968(u32, u32, u32, u32);
void sub_08045970(u32, u32, u32, u32);
void sub_08045978(u32, u32, u32, s32);
void sub_08045980(u32, void (*)(void));
void sub_080459F8(void);
void sub_080458C8(u32 p)
{
    sub_08219DD8(p, 0x44);
    sub_08045934(p, 0, 0x3c, 0, 0);
    sub_08045950(p, 0, 0, 10, 0x14, 0x14);
    sub_08045968(p, 8, 0x20, 8);
    sub_08045970(p, 0, 0, 0);
    sub_08045978(p, 0x848F, 0, -1);
    sub_08045980(p, sub_080459F8);
}
