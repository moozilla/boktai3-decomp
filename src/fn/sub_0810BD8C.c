#define SlotState_ResetInactiveMode sub_0810BD8C
#include "global.h"

struct SlotState {
    u8 unknown0[0x130];
    u8 mode[1];
};

void sub_0810BD8C(struct SlotState *p)
{
    s32 i;
    for (i = 0; i <= 0; i++) {
        if (i == 0) {
            if ((u8)(p->mode[i] - 6) > 4)
                p->mode[i] = i;
        }
    }
}
