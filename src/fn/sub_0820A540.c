#include "global.h"
#include "engine.h"
#include "ram.h"
struct Slot600 {
    u8 pad0[4];
    s32 name;
    u8 pad8[0x11C];
    u8 active, stopped;
    u8 pad126[0x5A];
};
struct Slot600Manager {
    u8 pad0[0x18];
    u32 used;
    struct Slot600 slots[8];
};
void *FanCannonManager_Create(void);
/* Script: in the eight-slot manager at 0x02000600 (created if missing, which
   fails the call), the used slot named -n clears both +0x124 and +0x125. */
s32 Slot600_ResetFromScript(void)
{
    struct Slot600Manager *manager = gFanCannonManager;
    struct Slot600 *slot;
    s32 name, i;
    if (!manager) {
        FanCannonManager_Create();
        return -1;
    }
    if (!Script_SeekToKeyword('n'))
        return -1;
    name = Script_GetValue();
    slot = manager->slots;
    for (i = 0; i <= 7; i++, slot++) {
        if ((1 << i) & manager->used && slot->name == name) {
            slot->active = 0;
            slot->stopped = 0;
            return 0;
        }
    }
    return -1;
}
