#include "global.h"
#include "runtime.h"
struct Entry {
    u16 id;
    u8 flags, pad;
    void *data;
};
extern struct Entry gUnk_0203B000[];
struct Entry *ResourceRegistry_AllocateEntry(void)
{
    s32 i;
    struct Entry *entry;
    do {
        for (entry = gUnk_0203B000, i = 0; i < gResourceRegistryCount; i++, entry++) {
            do {
                if (!(entry->flags & 0x80))
                    return entry;
            } while (0);
        }
        gResourceRegistryCount++;
        if (gResourceRegistryCount > 31)
            return 0;
    } while (0);
    return entry;
}
