#include "global.h"

struct Slot {
    u16 id;
    u8 flags;
    u8 pad;
    u32 val;
};
struct Slot *ResourceRegistry_FindEntry(u16);

u32 ResourceRegistry_GetResource(u16 id)
{
    struct Slot *s = ResourceRegistry_FindEntry(id);
    if (s == 0)
        return 0;
    return s->val;
}
