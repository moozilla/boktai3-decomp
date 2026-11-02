#include "global.h"
struct CallbackSlot {
    u8 unk0[0x60];
    u16 callbackIndex;
    u8 unk62[6];
};
struct CallbackSlots {
    u8 unk0[0x1c];
    struct CallbackSlot slots[3];
    u32 activeMask;
};
extern void *gUnk_08606530[];
s32 sub_08249240(void *, void *, void *);
static inline u8 IsSlotActive(u32 *used, s32 i)
{
    u32 bit = 1 << i;
    if (*used & bit)
        return 1;
    return 0;
}

static inline void CallbackSlots_DispatchOccupied(struct CallbackSlots *obj,
                                                  struct CallbackSlot *slots)
{
    s32 i;
    u32 *used;
    void **table;
    i = 0;
    used = &obj->activeMask;
    table = gUnk_08606530;
    do {
        if (IsSlotActive(used, i)) {
            struct CallbackSlot *slot = (void *)((u32)i * sizeof(*slots) + (u32)slots);
            sub_08249240(obj, slot, table[slot->callbackIndex]);
        }
        i++;
    } while (i < 3);
}

s32 CallbackSlots_DispatchActive(struct CallbackSlots *obj)
{
    struct CallbackSlot *slots = obj->slots;
    if (!obj->activeMask)
        return 0;
    CallbackSlots_DispatchOccupied(obj, slots);
    return 0;
}
