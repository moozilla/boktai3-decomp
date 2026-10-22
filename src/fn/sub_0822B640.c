#include "global.h"
struct Stimulus {
    u16 f0;
    u16 f2;
    u16 flags;
    u8 pad[10];
};
struct Context {
    u8 pad[0x20];
    struct Stimulus stimuli[8];
};
extern struct Context *gUnk_030025FC;
/* Per-frame stimulus aging: a slot posted this frame (flag 4) loses the flag;
   one that was not re-posted is cleared (id 0xFFFF, flags 0). */
void sub_0822B640(void)
{
    struct Context *ctx = gUnk_030025FC;
    s32 i;
    if (ctx != NULL) {
        for (i = 0; i < 8; i++) {
            u16 f = ctx->stimuli[i].flags & 4;
            if (f)
                ctx->stimuli[i].flags &= ~4;
            else {
                ctx->stimuli[i].f0 |= 0xFFFF;
                ctx->stimuli[i].flags = 0;
            }
        }
    }
}
