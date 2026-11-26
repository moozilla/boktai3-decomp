#include "global.h"
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
s32 CallbackPool_InvokeActive(struct CallbackPool *pool)
{
    s32 i;
    for (i = 0; i < 48; i++) {
        if (pool->entries[i].active)
            Callback_InvokeActor(&pool->entries[i], pool->entries[i].callback);
    }
    return 0;
}
