#include "global.h"
#include "ram.h"
struct Entry {
    u8 state;
    u8 pad[0x1f];
    u32 flags;
    u8 pad2[0x24];
};
struct Manager {
    u8 pad[0x1c];
    u16 mode;
    u8 pad2[0x12];
    struct Entry entries[40];
};

s32 sub_081D36D4(void)
{
    struct Manager *m = gCloudTask;
    struct Entry *e;
    u32 value;
    s32 i;
    if (m == NULL)
        return -1;
    e = m->entries;
    value = 1;
    for (i = 39; i >= 0; i--, e++)
        e->flags |= value;
    return 0;
}
