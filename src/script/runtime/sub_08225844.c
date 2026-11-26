#include "global.h"
struct Entry {
    u32 key;
    u32 callback;
};
s32 Div(s32, s32);
u32 Script_SearchActorRegistry(u32 key, struct Entry *table, s32 start, s32 end)
{
    while (start < end) {
        s32 middle = Div(start + end, 2);
        if (table[middle].key < key)
            start = middle + 1;
        else
            end = middle;
    }
    if (table[start].key == key)
        return table[start].callback;
    else
        return 0;
}
