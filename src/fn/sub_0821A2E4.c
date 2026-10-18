#include "global.h"

struct Buf {
    u32 a, b;
    u8 pad[0x180];
};
extern u32 gUnk_03001680;
struct Buf *MessageQueue_GetBuffers(void);

void MessageQueue_InitBuffers(void)
{
    struct Buf *b = MessageQueue_GetBuffers();
    gUnk_03001680 = 0;
    b[0].a = 0;
    b[0].b = 0;
    b[1].a = 0;
    b[1].b = 0;
}
