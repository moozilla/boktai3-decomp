#include "global.h"
void sub_0821AD08(u32, u32);
void sub_0804DC18(u8 *p)
{
    if (*(u32 *)(p + 0x64) == *(u32 *)(p + 0x54)) {
        if (*(u32 *)(p + 0x68) != 0) {
            sub_0821AD08(*(u32 *)(p + 0x68), 0);
            *(u32 *)(p + 0x68) = 0;
        }
    }
    *(u32 *)(p + 0x64) += 1;
}
