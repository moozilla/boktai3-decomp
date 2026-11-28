#include "engine/message_queue.h"
#include "engine/runtime.h"
#include "engine/video/render.h"

struct MessageQueueBuffer *MessageQueue_GetBuffers(void)
{
    return (struct MessageQueueBuffer *)0x0203F400;
}

struct MessageQueueBuffer *MessageQueue_GetBuffers(void);

void MessageQueue_InitBuffers(void)
{
    struct MessageQueueBuffer *b = MessageQueue_GetBuffers();
    gMessageQueueBufferIndex = 0;
    b[0].count = 0;
    b[0].used = 0;
    b[1].count = 0;
    b[1].used = 0;
}

void MessageQueue_SwapBuffers(void)
{
    struct MessageQueueBuffer *buffers = MessageQueue_GetBuffers();
    if (gEnginePauseFlags == 0) {
        struct MessageQueueBuffer *buffer;
        gMessageQueueBufferIndex ^= 1;
        buffer = &buffers[gMessageQueueBufferIndex];
        buffer->count = 0;
        buffer->used = 0;
    }
}

void MessageQueue_PostMessage(struct QueueMessage *message)
{
    struct MessageQueueBuffer *buffer = MessageQueue_GetBuffers() + gMessageQueueBufferIndex;
    struct QueueMessage *p = buffer->entries;
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
    p->reserved = 0;
    p->count = message->count;
    source = message->values;
    destination = buffer->values + buffer->used;
    p->values = destination;
    buffer->used += message->count;
    for (i = 0; i < message->count; i++)
        *destination++ = *source++;
}

s32 MessageQueue_GetMessagesById(u32 id, struct QueueMessage **out)
{
    struct MessageQueueBuffer *buffer = MessageQueue_GetBuffers() + (1 - gMessageQueueBufferIndex);
    struct QueueMessage *p = buffer->entries;
    s32 i;

    for (i = 0; i < buffer->count; i++, p++) {
        if (p->id == id) {
            s32 count;

            *out = p;
            p++;
            i++;
            count = 1;
            /* Messages with one id are adjacent. */
            if (i < buffer->count && p->id == id) {
                do {
                    i++;
                    count++;
                    p++;
                } while (i < buffer->count && p->id == id);
            }
            return count;
        }
    }
    return 0;
}
