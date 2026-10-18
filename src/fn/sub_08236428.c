#define CallbackGroup_UpdateActive sub_08236428
#include "global.h"
#include "engine.h"
struct CallbackGroupEntry {
    u8 unknown0[0x28];
    u8 active;
    u8 unknown29[0x17];
    void *callback;
};
struct CallbackGroup {
    u8 unknown0[0x13a];
    u8 active;
    u8 unknown13B;
    void *context;
    struct CallbackGroupEntry entries[12];
};
void sub_08236428(struct CallbackGroup *group)
{
    s32 count;
    u8 *active;
    struct CallbackGroupEntry *entry;
    s32 i;
    if (group->active) {
        count = 0;
        active = &group->entries[0].active;
        entry = group->entries;
        for (i = 11; i >= 0; i--) {
            if (*active) {
                sub_08249240(entry, group->context, *(void **)(active + 0x18));
                count++;
            }
            active += 0x44;
            entry++;
        }
        if (count == 0)
            group->active = count;
    }
}
