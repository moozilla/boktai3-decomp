#include "global.h"
struct CallbackList {
    u8 pad0[0xbd];
    u8 valueBase;
    u8 padBE[0x27c];
    u16 valueQuarter;
};
s32 CallbackList_RunMatchingState(struct CallbackList *, s32, s32);
s32 CallbackList_RunStateEight(struct CallbackList *list)
{
    s32 value = list->valueBase + (list->valueQuarter >> 2);
    value = CallbackList_RunMatchingState(list, value, 8);
    return value;
}
