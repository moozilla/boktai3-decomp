#include "global.h"
#define CallbackList_RunActive sub_0811EA78
struct CallbackEntry { void *function; u8 unk4; u8 state; u8 pad6[14]; };
struct CallbackList { u8 pad0[0x54]; u8 count; u8 pad55[0x7b]; struct CallbackEntry entries[1]; };
s32 sub_08249240(void *, void *, void *);
s32 sub_0811EA78(struct CallbackList *list, void *value)
{
    struct CallbackEntry *base = list->entries;
    s32 i = 0;
    s32 count = list->count;
    if (i < count) {
        struct CallbackEntry *entry = base;
        struct CallbackEntry *cursor = base;
        do {
            if (cursor->state == 1)
                value = (void *)sub_08249240(entry, value, cursor->function);
            cursor++;
            entry++;
            i++;
            count = list->count;
        } while (i < count);
    }
    return (s32)value;
}
