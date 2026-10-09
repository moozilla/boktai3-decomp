#include "global.h"
struct Entry { u16 id; u8 flags, pad; void *data; };
extern struct Entry gUnk_0203B000[];
extern s32 gUnk_030052F0;
void sub_08219D38(void *);
void sub_0821A218(u32 keep)
{
    s32 retained = 0;
    struct Entry *entry = gUnk_0203B000;
    s32 i = 0;
    while (i < gUnk_030052F0) {
        if (entry->flags & 0x80) {
            if ((entry->flags & 1) && !(keep & 1))
                retained = i;
            else {
                if (entry->flags & 2)
                    sub_08219D38(entry->data);
                entry->id = 0;
                entry->flags = 0;
            }
        }
        i++;
        entry++;
    }
    gUnk_030052F0 = retained + 1;
}
