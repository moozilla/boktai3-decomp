#include "global.h"
#include "render.h"
#include "runtime.h"

struct Buf {
    u32 a, b;
    u8 pad[0x180];
};
struct Buf *MessageQueue_GetBuffers(void);

void MessageQueue_SwapBuffers(void)
{
    struct Buf *b = MessageQueue_GetBuffers();
    if (gEnginePauseFlags == 0) {
        struct Buf *e;
        gMessageQueueBufferIndex ^= 1;
        e = (struct Buf *)((u8 *)b + gMessageQueueBufferIndex * 0x188);
        e->a = 0;
        e->b = 0;
    }
}
