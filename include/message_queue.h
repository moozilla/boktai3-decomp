#ifndef GUARD_MESSAGE_QUEUE_H
#define GUARD_MESSAGE_QUEUE_H
#include "runtime.h"
struct QueueMessage {
    u16 id;
    u8 reserved, count;
    s16 *values;
};
struct MessageQueueBuffer {
    s32 count, used;
    struct QueueMessage entries[32];
    s16 values[64];
};
struct MessageQueueBuffer *MessageQueue_GetBuffers(void);
void MessageQueue_InitBuffers(void);
void MessageQueue_SwapBuffers(void);
void MessageQueue_PostMessage(struct QueueMessage *message);
s32 MessageQueue_GetMessagesById(u32 id, struct QueueMessage **out);
#endif
