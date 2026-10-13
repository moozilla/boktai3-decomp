#include "global.h"
struct Slot {
    u8 active;
    u8 pad1[7];
    s16 timeout;
    u8 padA[6];
    u16 id;
    u8 pad12[218];
};
struct Manager {
    u8 pad[24];
    u32 count;
    struct Slot *slots;
};
extern struct Manager *gUnk_02000034;
s32 sub_0821ABA8(u32, s32);
#define Breakable_ScriptSetTimeout sub_08009454
void Breakable_ScriptSetTimeout(void)
{
    struct Manager *manager = gUnk_02000034;
    if (manager) {
        s32 id = sub_0821ABA8('n', 0), timeout = sub_0821ABA8('d', -1);
        struct Slot *slot = manager->slots;
        u32 i;
        for (i = 0; i < manager->count; i++, slot++)
            if (slot->active == 1 && slot->id == id)
                slot->timeout = timeout;
    }
}
