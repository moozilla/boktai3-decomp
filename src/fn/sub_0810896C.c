#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.3%
// foundry draft (worker best 90.2%); what differs is mostly register allocation and instruction order
#define CallbackSlots_DispatchActive sub_0810896C
struct CallbackSlot { u8 unk0[0x60]; u16 callbackIndex; u8 unk62[6]; };
struct CallbackSlots { u8 unk0[0x1c]; struct CallbackSlot slots[3]; u32 activeMask; };
extern void *gUnk_08606530[];
s32 sub_08249240(void *, void *, void *);
static inline u8 IsSlotActive(struct CallbackSlot *slots, s32 i)
{
    u32 bit = 1 << i;
    if (*(u32 *)(slots + 3) & bit) return 1;
    return 0;
}
s32 sub_0810896C(struct CallbackSlots *obj)
{
    struct CallbackSlot *slots = obj->slots;
    s32 i;
    if (*(u32 *)((u8 *)obj + 0x154)) {
        for (i = 0; i < 3; i++) {
            if (IsSlotActive(slots, i))
                sub_08249240(obj, (struct CallbackSlot *)((u32)(i * sizeof(*slots)) + (u32)slots), gUnk_08606530[((struct CallbackSlot *)((u32)(i * sizeof(*slots)) + (u32)slots))->callbackIndex]);
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810896C);
#endif
