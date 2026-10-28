#include "global.h"
#include "runtime.h"

struct Slot {
    u16 id;
    u8 flags;
    u8 pad;
    u32 val;
};
extern struct Slot gUnk_0203B000[];

struct Slot *ResourceRegistry_FindEntry(u16 id)
{
    struct Slot *s = gUnk_0203B000;
    s32 i = 0;
    for (; i < gResourceRegistryCount; i++, s++) {
        if ((s->flags & 0x80) != 0 && s->id == id)
            return s;
    }
    return 0;
}
