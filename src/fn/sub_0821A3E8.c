#include "global.h"
#include "runtime.h"
struct Message {
    u16 id;
    u8 unk2, count;
    s16 *values;
};
struct Buffer {
    s32 count, used;
    struct Message entries[32];
    s16 values[64];
};
struct Buffer *MessageQueue_GetBuffers(void);
s32 MessageQueue_GetMessagesById(u32 id, struct Message **out)
{
    struct Buffer *buffer = MessageQueue_GetBuffers() + (1 - gMessageQueueBufferIndex);
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
