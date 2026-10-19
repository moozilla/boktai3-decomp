#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.8%
// foundry draft (worker best 81.8%); what differs is mostly register allocation and instruction order
#define CallbackPool_InvokeActive sub_081158B8
struct CallbackEntry {
    u8 payload[0xF0];
    u8 active;
    u8 padF1[3];
    void *callback;
};
struct CallbackPool {
    u8 pad0[0x18];
    struct CallbackEntry entries[48];
};
s32 Callback_InvokeActor(void *, void *);
s32 sub_081158B8(struct CallbackPool *pool)
{
    struct CallbackEntry *entry = pool->entries;
    u8 *active = &pool->entries[0].active;
    s32 index = 0;
    s32 count = 47;
    for (; count >= 0; count--, index++) {
        if (*active) {
            Callback_InvokeActor(entry, pool->entries[index].callback);
        }
        entry++;
        active += 0xF8;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081158B8);
#endif
