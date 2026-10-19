#include "global.h"
#ifdef NONMATCHING
// fuzzy: 94.1%
// foundry draft (worker best 94.1%); what differs is mostly register allocation and instruction order
#define CallbackTask_Update sub_08108E5C
#define gTaskInput gUnk_03005260
struct CallbackTask {
    u8 pad0[0x1C];
    u32 cancellable;
    s32 arg2;
    s32 arg1;
    u32 result;
    u32 completed;
    u32 acknowledged;
    u8 pad34[0x6F - 0x34];
    u8 arg3;
    u32 flags;
    void (*success)(void *, s32, s32, u32);
    void (*failure)(void *, s32, s32, u32);
    void (*update)(struct CallbackTask *);
    void *context;
};
extern u16 gUnk_03005260[];
u8 CallbackTask_Finish(struct CallbackTask *);
s32 sub_08108E5C(struct CallbackTask *task)
{
    if (CallbackTask_Finish(task)) return 0;
    task->update(task);
    if (!task->cancellable) return 0;
    if (task->completed) return 0;
    if (task->result) return 0;
    if (gUnk_03005260[1] & 2) {
        task->completed = 1;
        task->arg2 = 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08108E5C);
#endif
