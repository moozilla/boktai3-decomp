#include "global.h"
struct CallbackList {
    u8 pad0[0x58];
    s16 value;
};
struct CallbackReduction {
    u8 pad0[8];
    u16 amount;
};
s32 CallbackList_RunMatchingState(struct CallbackList *, s32, s32);
void CallbackList_SubtractStateTenValue(struct CallbackList *list,
                                        struct CallbackReduction *reduction, s32 amount)
{
    if (amount <= 0) {
        amount = reduction->amount;
        amount = CallbackList_RunMatchingState(list, amount, 10);
    }
    list->value -= amount;
    if (list->value < 0)
        list->value = 0;
}
