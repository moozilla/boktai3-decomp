#include "global.h"
#ifdef NONMATCHING
// fuzzy: 83.3%
// foundry draft (worker best 80.0%); what differs is mostly register allocation and instruction order
#define CallbackList_RunMatchingState sub_0811EAF8
struct CallbackEntry {
    void *function;
    u8 unk4;
    u8 state;
    u8 pad6[14];
};
struct CallbackList {
    u8 pad0[0x54];
    u8 count;
    u8 pad55[0x7b];
    struct CallbackEntry entries[1];
};
s32 sub_08249240(void *, void *, void *);
s32 sub_0811EAF8(struct CallbackList *list, void *value, s32 state)
{
    struct CallbackEntry *base = list->entries;
    s32 i = 0;
    if (i < list->count) {
        struct CallbackEntry *entry = base;
        struct CallbackEntry *cursor = base;
        do {
            if (cursor->state == state || cursor->state == 13)
                value = (void *)sub_08249240(entry, value, cursor->function);
            cursor++;
            entry++;
            i++;
        } while (i < list->count);
    }
    return (s32)value;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811EAF8);
#endif
