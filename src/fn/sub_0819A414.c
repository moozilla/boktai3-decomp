#include "global.h"
struct CallbackSlotD4 {
    u32 active;
    void (*release)(struct CallbackSlotD4 *);
    u8 pad8[0xD4 - 8];
};
struct CallbackSlotB8 {
    u32 active;
    void (*release)(struct CallbackSlotB8 *);
    u8 pad8[0xB8 - 8];
};
struct CallbackSlots {
    u8 pad0[0x28];
    struct CallbackSlotD4 first[16];
    u8 padD68[0xD68 - 0x28 - 16 * 0xD4];
    struct CallbackSlotB8 second[8];
};
// confidence=low: clears both slot pools, calling each slot's release callback once.
#ifdef NONMATCHING
// fuzzy: 78.6%
// The ROM multiplies into a copy of the index; this multiplies into the size constant.
s32 sub_0819A414(struct CallbackSlots *p)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        struct CallbackSlotD4 *slot = (void *)((u8 *)p + i * sizeof(*slot) + 0x28);
        slot->active = 0;
        if (slot->release) {
            slot->release(slot);
            slot->release = 0;
        }
    }
    for (i = 0; i < 8; i++) {
        struct CallbackSlotB8 *slot = (void *)((u8 *)p + i * sizeof(*slot) + 0xD68);
        slot->active = 0;
        if (slot->release) {
            slot->release(slot);
            slot->release = 0;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0819A414);
#endif
