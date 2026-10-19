#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 85.7%
// foundry draft (worker best 83.6%); what differs is mostly register allocation and instruction order
#define SceneStatusIndicator_UpdateManager sub_081268A4
struct SceneStatusIndicatorSlot { u8 unknown0[0x2A]; u8 active, index; u16 reset, timer; };
struct SceneStatusIndicatorManager { u8 unknown0[0x1C]; struct SceneStatusIndicatorSlot slots[12]; u32 allocated; };
extern void *gUnk_08610758[];
static inline u8 allocated(u32 *flags, s32 index)
{
    if (*flags & (1 << index)) return 1;
    return 0;
}
s32 sub_081268A4(struct SceneStatusIndicatorManager *manager)
{
    struct SceneStatusIndicatorSlot *start = manager->slots;
    struct SceneStatusIndicatorSlot *entry;
    s32 i;
    u32 *flags;
    void **callbacks;
    u8 *active;
    if (manager->allocated) {
        i = 0;
        flags = (u32 *)((u32)manager + 0x25C);
        callbacks = gUnk_08610758;
        active = &manager->slots[0].active;
        entry = start;
        do {
            if (allocated(flags, i)) sub_08249240(manager, entry, callbacks[*active]);
            active += 0x30;
            entry++;
            i++;
        } while (i <= 11);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081268A4);
#endif
