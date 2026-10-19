#include "global.h"
#ifdef NONMATCHING
// fuzzy: 80.4%
// foundry draft (worker best 80.4%); what differs is mostly register allocation and instruction order
#define CallbackPool_Destroy sub_08199314
#define Callback_Invoke Callback_InvokeActor
#define gCallbackPool gUnk_02000218
struct CallbackPoolEntry { u32 active; void *destroy; u8 unknown[0x150]; };
struct CallbackPoolSlot { u32 active; void *destroy; u8 unknown[0xb0]; };
struct CallbackPool { u8 unknown[0x34]; struct CallbackPoolEntry entries[1]; struct CallbackPoolSlot slots[4]; };
struct CallbackPoolLargeRecord { u8 prefix[0x34]; u32 active; void *destroy; u8 tail[0x11c]; };
struct CallbackPoolSmallRecord { u8 prefix[0x18c]; u32 active; void *destroy; };
extern struct CallbackPool *gUnk_02000218;
void Callback_InvokeActor(void *, void *);
s32 sub_08199314(struct CallbackPool *pool)
{
    s32 i;
    for (i = 0; i <= 0; i++) {
        struct CallbackPoolEntry *entry = (struct CallbackPoolEntry *)&((struct CallbackPoolLargeRecord *)pool)[i].active;
        entry->active = 0;
        if (entry->destroy) {
            Callback_InvokeActor(entry, entry->destroy);
            entry->destroy = 0;
        }
    }
    for (i = 0; i <= 3; i++) {
        struct CallbackPoolSlot *slot = (struct CallbackPoolSlot *)(0xb8 * i + (u32)pool + 0x18c);
        slot->active = 0;
        if (slot->destroy) {
            Callback_InvokeActor(slot, slot->destroy);
            slot->destroy = 0;
        }
    }
    return (u32)(gUnk_02000218 = 0);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08199314);
#endif
