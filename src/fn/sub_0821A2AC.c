#include "global.h"

struct Slot {
    u16 id;
    u8 flags;
    u8 pad;
    u32 val;
};
struct Slot *ResourceRegistry_FindEntry(u16);

void ResourceRegistry_UnregisterResource(u16 id)
{
    struct Slot *s = ResourceRegistry_FindEntry(id);
    if (s != 0)
        s->flags = 0;
}
