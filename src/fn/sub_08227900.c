#include "global.h"
struct ResourceLookupEntry {
    u16 key, value;
};
u32 ResourceLookup_FindValue(u32 key, struct ResourceLookupEntry *entries, s32 low, s32 high)
{
    while (low < high) {
        s32 middle = (low + high) >> 1;
        if (entries[middle].key < key)
            low = middle + 1;
        else
            high = middle;
    }
    if (entries[low].key == key)
        return entries[low].value;
    else
        return 0;
}
