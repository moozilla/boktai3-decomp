#include "global.h"
static inline s32 State(u8 *p)
{
    return *(s16 *)(p + 0x86);
}

void sub_081C0A24(u8 *p)
{
    if (*(u32 *)(p + 0x1c) & 1)
        return;
    switch (*(s16 *)(p + 0x86)) {
        case 0:
            if (++*(s16 *)(p + 0x84) > 1) {
                *(s16 *)(p + 0x2c) = State(p);
                *(s16 *)(p + 0x86) = 1;
                *(s16 *)(p + 0x84) = 0;
            }
            break;
        case 1:
            if (++*(s16 *)(p + 0x84) > 0) {
                *(s16 *)(p + 0x2c) = State(p);
                *(s16 *)(p + 0x86) = 0;
                *(s16 *)(p + 0x84) = 0;
            }
            break;
    }
}