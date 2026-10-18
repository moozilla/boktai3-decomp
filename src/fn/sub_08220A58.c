#define PositionQueue_FlushPending sub_08220A58
#define gEnginePauseFlags          gUnk_0300523C
#define Collision_ProcessPairs     sub_08220938
#include "global.h"
extern u32 gUnk_0300523C;
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
void sub_08220938(void *, void *);
s32 sub_08220A58(struct PositionQueue *q)
{
    if (q->ready) {
        if (gUnk_0300523C) {
            u32 *pending;
            q->ready = 0;
            pending = q->pending;
            *pending = 0;
            q->pendingTail = pending;
        } else {
            u32 *pending;
            if (q->count)
                sub_08220938(q->pending, q->source);
            q->ready = 0;
            pending = q->pending;
            *pending = 0;
            q->pendingTail = pending;
        }
    }
    return 0;
}
