#include "global.h"
#define CallbackList_ResetDispatch sub_0811EE0C
struct CallbackDispatch {
    void (*dispatch)(void);
    u8 padB4[8];
    s8 slotIndex;
    u8 valueBase;
    u8 dispatchState;
};
struct CallbackList { u8 pad0[0xb0]; struct CallbackDispatch control; };
void TimedSpriteEffect_RequestStop(u32);
void Modifier_UpdateIdleDrain(void);
void sub_0811EE0C(struct CallbackList *list)
{
    if (list->control.slotIndex >= 0)
        TimedSpriteEffect_RequestStop(list->control.slotIndex);
    list->control.slotIndex = -1;
    {
        struct CallbackDispatch *control = &list->control;
        control->dispatch = Modifier_UpdateIdleDrain;
        control->dispatchState = 1;
    }
}
