#include "global.h"
struct Entry {
    u16 id;
    u8 flags, pad;
    void *data;
};
struct Entry *ResourceRegistry_AllocateEntry(void);
void ResourceRegistry_RegisterResource(u32 id, void *data, u32 flags)
{
    void *saved_data = data;
    s32 saved_flags = flags;
    u16 saved_id = id;
    struct Entry *entry = ResourceRegistry_AllocateEntry();
    if (entry) {
        entry->id = saved_id;
        entry->data = saved_data;
        entry->flags = saved_flags | (s8)0x80;
    }
}
