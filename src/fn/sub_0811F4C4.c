#include "global.h"
#define CallbackStatus_ResetSelectorAndTest sub_0811F4C4
#define CallbackStatus_IsActiveWithoutCount Modifier_IsFourthSourceReady
struct CallbackStatus {
    u8 pad0[0x10];
    u8 active;
    u8 selector;
    u8 pad12[2];
    u16 count;
};
u32 Modifier_IsFourthSourceReady(struct CallbackStatus *);
u32 sub_0811F4C4(struct CallbackStatus *status)
{
    u32 result;
    status->selector = 0xff;
    result = Modifier_IsFourthSourceReady(status);
    /* The nonzero path preserves r0 through the original nonvoid fallthrough. */
    if ((u8)result == 0)
        return 0;
}
