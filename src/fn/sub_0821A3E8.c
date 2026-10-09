#include "global.h"
struct Message { u16 id; u8 unk2, count; s16 *values; };
struct Buffer { s32 count, used; struct Message entries[32]; s16 values[64]; };
extern s32 gUnk_03001680;
struct Buffer *sub_0821A2DC(void);
s32 sub_0821A3E8(u32 id, struct Message **out)
{
    struct Buffer *buffer = sub_0821A2DC() + (1 - gUnk_03001680);
    struct Message *p = buffer->entries;
    s32 i = 0;
    s32 count, limit;
    while (limit = buffer->count, i < limit) {
        if (p->id == id) {
            *out = p;
            p++;
            i++;
            count = 1;
            if (i < limit && p->id == id) {
                limit = buffer->count;
                do {
                    i++;
                    count++;
                    p++;
                } while (i < limit && p->id == id);
            }
            return count;
        }
        i++;
        p++;
    }
    return 0;
}
