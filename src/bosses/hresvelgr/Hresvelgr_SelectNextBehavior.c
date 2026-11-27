#include "global.h"
#include "bosses/hresvelgr/offset_actor_layout.h"
extern const u32 gUnk_08611D44[], gUnk_08611D5C[], gUnk_08611D7C[], gUnk_08611D8C[];
// confidence=low: advances behavior tables on completed actions, with health and contact transitions.
void Hresvelgr_SelectNextBehavior(struct OffsetActor *p)
{
    struct OffsetActor *q = p;
    u32 next;
    if (!p->ready)
        return;
    if (!p->index)
        p->response = 0;
    switch (p->phase) {
        case 0: {
            u32 next;
            if (p->index <= 4)
                next = p->index + 1;
            else
                next = 0;
            q->index = next;
            if (q->contact) {
                q->phase = 2;
                q->index = 0;
                p->behavior = gUnk_08611D7C[0];
                p->behaviorTick = 0;
                p->pending = 1;
            } else if (p->health < (p->maxHealth >> 1)) {
                q->phase = 1;
                q->index = 0;
                p->behavior = gUnk_08611D5C[0];
                p->behaviorTick = 0;
                p->pending = 1;
            } else {
                p->behavior = gUnk_08611D44[q->index];
                p->behaviorTick = 0;
                p->pending = 1;
            }
            break;
        }
        case 1:
            if (p->contact) {
                p->phase = 3;
                p->index = 0;
                p->behavior = gUnk_08611D8C[0];
                p->behaviorTick = 0;
                p->pending = 1;
            } else {
                if (p->index <= 6)
                    next = p->index + 1;
                else
                    next = 0;
                q->index = next;
                p->behavior = gUnk_08611D5C[q->index];
                p->behaviorTick = 0;
                p->pending = 1;
            }
            break;
        case 2:
            if (p->index + 1 > 3U) {
                p->phase = 0;
                p->index = 0;
                p->behavior = gUnk_08611D44[0];
                p->behaviorTick = 0;
                p->pending = 1;
            } else {
                p->index++;
                p->behavior = gUnk_08611D7C[p->index];
                p->behaviorTick = 0;
                p->pending = 1;
            }
            break;
        case 3:
            if (p->index + 1 > 7U) {
                p->phase = 1;
                p->index = 0;
                p->behavior = gUnk_08611D5C[0];
                p->behaviorTick = 0;
                p->pending = 1;
            } else {
                p->index++;
                p->behavior = gUnk_08611D8C[p->index];
                p->behaviorTick = 0;
                p->pending = 1;
            }
            break;
    }
}
