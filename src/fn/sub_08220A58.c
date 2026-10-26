#include "global.h"
#include "render.h"
struct PositionQueue {
    u8 unknown0[0x18];
    void *source;
    u32 *pending;
    void *sourceTail;
    u32 *pendingTail;
    u8 unknown28[0xA8];
    u16 count;
    u16 ready;
};
void Collision_ProcessPairs(void *, void *);
s32 PositionQueue_FlushPending(struct PositionQueue *q)
{
    if (q->ready) {
        if (gEnginePauseFlags) {
            u32 *pending;
            q->ready = 0;
            pending = q->pending;
            *pending = 0;
            q->pendingTail = pending;
        } else {
            u32 *pending;
            if (q->count)
                Collision_ProcessPairs(q->pending, q->source);
            q->ready = 0;
            pending = q->pending;
            *pending = 0;
            q->pendingTail = pending;
        }
    }
    return 0;
}
