#include "global.h"

struct SlotState {
    u8 unknown0[0x130];
    u8 mode[1];
};

void SlotState_ResetInactiveMode(struct SlotState *p)
{
    s32 i;
    for (i = 0; i < 1; i++) {
        if (i == 0) {
            if (p->mode[i] < 6 || p->mode[i] > 10)
                p->mode[i] = 0;
        }
    }
}
