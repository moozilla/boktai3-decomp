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
void MessageQueue_PostMessage(struct Message *message)
{
    struct Buffer *buffer = MessageQueue_GetBuffers() + gMessageQueueBufferIndex;
    struct Message *p = buffer->entries;
    s32 insert = buffer->count;
    s32 i;
    s16 *source, *destination;
    for (i = 0; i < buffer->count; i++, p++) {
        if (p->id == message->id)
            insert = i + 1;
    }
    for (i = buffer->count; i > insert; i--)
        buffer->entries[i] = buffer->entries[i - 1];
    buffer->count++;
    p = &buffer->entries[insert];
    p->id = message->id;
    p->unk2 = 0;
    p->count = message->count;
    source = message->values;
    destination = buffer->values + buffer->used;
    p->values = destination;
    buffer->used += message->count;
    for (i = 0; i < message->count; i++)
        *destination++ = *source++;
}
