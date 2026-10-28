#include "global.h"
#include "runtime.h"

struct Buf {
    u32 a, b;
    u8 pad[0x180];
};
struct Buf *MessageQueue_GetBuffers(void);

void MessageQueue_InitBuffers(void)
{
    struct Buf *b = MessageQueue_GetBuffers();
    gMessageQueueBufferIndex = 0;
    b[0].a = 0;
    b[0].b = 0;
    b[1].a = 0;
    b[1].b = 0;
}
