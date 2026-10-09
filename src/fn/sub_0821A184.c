#include "global.h"
struct Entry { u16 id; u8 flags, pad; void *data; };
extern struct Entry gUnk_0203B000[];
extern s32 gUnk_030052F0;
struct Entry *sub_0821A184(void)
{
    s32 i;
    struct Entry *entry;
    do {
        for (entry = gUnk_0203B000, i = 0; i < gUnk_030052F0; i++, entry++) {
            do {
                if (!(entry->flags & 0x80))
                    return entry;
            } while (0);
        }
        gUnk_030052F0++;
        if (gUnk_030052F0 > 31)
            return 0;
    } while (0);
    return entry;
}
