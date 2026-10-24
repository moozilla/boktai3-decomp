#include "global.h"
#define Callback_Invoke Callback_InvokeActor
#define gCallbackPool   gUnk_02000218
struct CallbackPoolEntry {
    u32 active;
    void *destroy;
    u8 unknown[0x150];
};
struct CallbackPoolSlot {
    u32 active;
    void *destroy;
    u8 unknown[0xb0];
};
struct CallbackPool {
    u8 unknown[0x34];
    struct CallbackPoolEntry entries[1];
    struct CallbackPoolSlot slots[4];
};
extern struct CallbackPool *gUnk_02000218;
void Callback_InvokeActor(void *, void *);
s32 CallbackPool_Destroy(struct CallbackPool *pool)
{
    s32 i;
    for (i = 0; i <= 0; i++) {
        u32 address = i;
        struct CallbackPoolEntry *entry;
        address *= sizeof(struct CallbackPoolEntry);
        address += (u32)pool;
        address += sizeof(pool->unknown);
        entry = (struct CallbackPoolEntry *)address;
        entry->active = 0;
        if (entry->destroy) {
            Callback_InvokeActor(entry, entry->destroy);
            entry->destroy = 0;
        }
    }
    for (i = 0; i <= 3; i++) {
        u32 address = i;
        struct CallbackPoolSlot *slot;
        address *= sizeof(struct CallbackPoolSlot);
        address += (u32)pool;
        address += sizeof(pool->unknown) + sizeof(pool->entries);
        slot = (struct CallbackPoolSlot *)address;
        slot->active = 0;
        if (slot->destroy) {
            Callback_InvokeActor(slot, slot->destroy);
            slot->destroy = 0;
        }
    }
    return (u32)(gUnk_02000218 = 0);
}
