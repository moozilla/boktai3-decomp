#include "global.h"
#define CallbackList_SubtractStateTenValue sub_0811F490
#define CallbackList_RunMatchingState sub_0811EAF8
struct CallbackList { u8 pad0[0x58]; s16 value; };
struct CallbackReduction { u8 pad0[8]; u16 amount; };
s32 sub_0811EAF8(struct CallbackList *, s32, s32);
void sub_0811F490(struct CallbackList *list, struct CallbackReduction *reduction, s32 amount)
{
    if (amount <= 0) {
        amount = reduction->amount;
        amount = sub_0811EAF8(list, amount, 10);
    }
    list->value -= amount;
    if (list->value < 0)
        list->value = 0;
}
