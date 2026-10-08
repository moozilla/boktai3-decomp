#include "global.h"

u32 sub_08245890(void);

s32 sub_08224D78(void) {
    if (sub_08245890() == 0x8001)
        return 1;
    if (*(u32 *)0x03004264 > 0x1C1)
        return -1;
    return 0;
}
