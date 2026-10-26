#include "global.h"
#include "render.h"

struct Buf {
    u32 a, b;
    u8 pad[0x180];
};
extern u32 gUnk_03001680;
struct Buf *MessageQueue_GetBuffers(void);

void MessageQueue_SwapBuffers(void)
{
    struct Buf *b = MessageQueue_GetBuffers();
    if (gEnginePauseFlags == 0) {
        struct Buf *e;
        gUnk_03001680 ^= 1;
        e = (struct Buf *)((u8 *)b + gUnk_03001680 * 0x188);
        e->a = 0;
        e->b = 0;
    }
}
